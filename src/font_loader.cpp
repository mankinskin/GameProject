#include "font_loader.h"
#define _USE_MATH_DEFINES
#include "shader.h"
#include "texture.h"
#include "gldebug.h"
#include "camera.h"
#include "vao.h"
#include "debug.h"
#include <algorithm>
#include "../dt/dt.h"
#include "../dt/image.h"
#include <fstream>
#include "viewport.h"
#include "font.h"

#define DEFAULT_TTF_DIR "../assets//fonts//"
#define DEFAULT_STORE_DIR "../assets//glyph_atlas_fonts//"

struct LoadFontMetrics 
{
	size_t ppem_x;
	size_t ppem_y;
	size_t scale_x;
	size_t scale_y;
	size_t lineGap;
};

struct LoadGlyphQuad 
{
	size_t minX;
	size_t minY;
	size_t maxX;
	size_t maxY;
};

struct LoadGlyphMetrics 
{
	size_t advanceX;
	int xBearing;
	int yBearing;
};

struct LoadAtlas 
{
	size_t width;
	size_t height;
	std::vector<LoadGlyphQuad> quads;
	std::vector<unsigned char> buffer;
};

struct LoadFont 
{
	std::string fileName;
	gui::text::FontInstructions instructions;
	LoadFontMetrics fontMetrics;
	LoadAtlas atlas;
	std::vector<LoadGlyphMetrics> metrics;
	std::vector<int> kerningMap;
};

std::vector<LoadFont> loadFontBuffer = {};
std::string font_input_directory = DEFAULT_TTF_DIR;
std::string font_store_directory = DEFAULT_STORE_DIR;
FT_Library ftLib;

std::vector<int> getKerningMap( FT_Face pFace, size_t pStartCode, size_t pGlyphCount )
{
	std::vector<int> kerning( pGlyphCount*pGlyphCount );
	//Get Kerning
	if ( FT_HAS_KERNING( pFace ) ) {
		for ( size_t g = 0; g < pGlyphCount; ++g ) {
			for ( size_t t = 0; t < pGlyphCount; ++t ) {
				FT_Vector vec = FT_Vector();
				FT_Get_Kerning( pFace, FT_Get_Char_Index( pFace, pStartCode + g ), FT_Get_Char_Index( pFace, pStartCode + t ), 0, &vec );
				if ( vec.x ) {
					vec.x = vec.x;
				}
				kerning[g*pGlyphCount + t] = ( vec.x ) / 64;
			}
		}
	}
	return kerning;
}

void setFontSize( FT_Face& pFace, LoadFontMetrics& pFontMetrics, gui::text::FontInstructions& pInstructions ) 
{
	size_t dpi_x = 100;
	size_t dpi_y = 100;
	if ( pInstructions.flags & 1 ) {
		dpi_x *= pInstructions.upsampling;
		dpi_y *= pInstructions.upsampling;
	}
	FT_Set_Char_Size( pFace, pInstructions.pointSize << 6, 0, dpi_x, dpi_y );
	//FT_Set_Pixel_Sizes( pFace, pInstructions.pointSize * pInstructions.upsampling, 0 );
	pFontMetrics.scale_x = pFace->size->metrics.x_scale;
	pFontMetrics.scale_y = pFace->size->metrics.y_scale;
	pFontMetrics.ppem_x = pFace->size->metrics.x_ppem;
	pFontMetrics.ppem_y = pFace->size->metrics.y_ppem;
	//pFontMetrics.lineGap = FT_MulFix( pFace->size->metrics.ascender - pFace->size->metrics.descender, pFace->size->metrics.y_scale )/64;
}

void loadAtlas( FT_Face& pFace, gui::text::FontInstructions& pFontInstructions, 
        LoadFontMetrics& pFontMetrics, LoadAtlas& pAtlas, std::vector<LoadGlyphMetrics>& pMetrics )
{
	const size_t width_glyphs = ( size_t )ceil( sqrt( ( float )pFontInstructions.glyphCount ) );
	size_t pad_pixels = 0;
	pad_pixels += pad_pixels % 2;
	size_t maxGlyphWidth = FT_MulFix( pFace->bbox.xMax - pFace->bbox.xMin, pFontMetrics.scale_x ) / 64; //max width of the actual glyph bitmap
	size_t maxGlyphHeight = FT_MulFix( pFace->bbox.yMax - pFace->bbox.yMin, pFontMetrics.scale_y ) / 64;//max height of the actual glyph bitmap

	const size_t atlasMaxAdvanceX = maxGlyphWidth + pad_pixels * 2;
	const size_t atlasMaxAdvanceY = maxGlyphHeight + pad_pixels * 2;

	const size_t preWidth = ( atlasMaxAdvanceX * width_glyphs );
	const size_t preHeight = ( atlasMaxAdvanceY * width_glyphs );

	std::vector<unsigned char> buf( preWidth*preHeight );
	pAtlas.quads.resize( pFontInstructions.glyphCount );
	pMetrics.resize( pFontInstructions.glyphCount );

	size_t maxAtlasWidth = 0; //the maximum width of the final atlas ( e.g. the maximum line width measured while loading the glyphs line-by-line into the atlas
	size_t thisLineMaxY = 0; // the maximum height of the current line( e.g. the distance to advance by when advancing to the next line )

	glm::uvec2 cursor = glm::uvec2();
	int maxAscend = 0; //for taking an accurate linegap distance. take the max value of all glyphs for these
	int maxDescend = 0;
	for ( size_t g = 0; g < pFontInstructions.glyphCount; ++g ) {
		//FreeType loads binary bitmap with 1 bit per pixel
		FT_Load_Char( pFace, pFontInstructions.startCode + g, FT_LOAD_RENDER );
		FT_GlyphSlot& gly = pFace->glyph;

		glm::uvec2 outlineSize = glm::uvec2( gly->bitmap.width, gly->bitmap.rows ); 
		LoadGlyphMetrics met;
		met.advanceX = size_t( ( gly->metrics.horiAdvance >> 6 ) );
		met.xBearing = size_t( ( gly->metrics.horiBearingX >> 6 ) );//gly->bitmap_left; //
		met.yBearing = size_t( ( gly->metrics.horiBearingY >> 6 ) );//gly->bitmap_top; //


		LoadGlyphQuad qud;
		qud.minX = cursor.x + pad_pixels;
		qud.minY = cursor.y + pad_pixels;
		qud.maxX = qud.minX + gly->bitmap.width;
		qud.maxY = qud.minY + gly->bitmap.rows;


		for ( size_t line = 0; line < gly->bitmap.rows; ++line ) {
			std::memcpy( &buf[( ( line + qud.minY )*preWidth ) + qud.minX], 
					&gly->bitmap.buffer[( line*gly->bitmap.width )], 
					sizeof( unsigned char )*gly->bitmap.width );
		}

		//advance atlas cursor
		cursor.x += qud.maxX - qud.minX;;
		thisLineMaxY = std::max( qud.maxY - qud.minY, thisLineMaxY );
		maxAscend = std::max( maxAscend, met.yBearing );
		maxDescend = std::max( maxDescend, ( int )( outlineSize.y ) - ( met.yBearing ) );
		pAtlas.quads[g] = qud;
		pMetrics[g] = met;

		//check whether to start a new line of glyphs in the atlas
		if ( ( cursor.x + ( atlasMaxAdvanceX ) ) > preWidth ) {
			maxAtlasWidth = std::max( maxAtlasWidth, ( size_t )cursor.x );
			cursor.x = 0;
			cursor.y += thisLineMaxY;
			thisLineMaxY = 0;
		}
	}
	
	maxAtlasWidth = std::max( maxAtlasWidth, ( size_t )cursor.x );
	cursor.y += thisLineMaxY;
	pAtlas.width = maxAtlasWidth;
	pAtlas.height = cursor.y;

	const size_t totalPixelCount = pAtlas.width * pAtlas.height;
	pAtlas.buffer.resize( totalPixelCount );
	pFontMetrics.lineGap = maxAscend + maxDescend;
	for ( size_t h = 0; h < pAtlas.height; ++h ) {
		std::memcpy( &pAtlas.buffer[h*pAtlas.width], &buf[h*preWidth], sizeof( unsigned char )*pAtlas.width );
	}
}

void loadFont( LoadFont& pFont )
{
	FT_Face ftface;
	std::string fontPath = font_input_directory + pFont.fileName + ".ttf";
	gui::text::FontInstructions& font_instructions = pFont.instructions;

	FT_New_Face( ftLib, ( fontPath ).c_str(), 0, &ftface );

	pFont.instructions.pointSize = font_instructions.pointSize;

	setFontSize( ftface, pFont.fontMetrics, pFont.instructions );
	loadAtlas( ftface, font_instructions, pFont.fontMetrics, pFont.atlas, pFont.metrics );
	pFont.kerningMap = getKerningMap( ftface, font_instructions.startCode, font_instructions.glyphCount );

	FT_Done_Face( ftface );
}

void gui::text::initializer::setFontInputDir( std::string pNewDirectory )
{
	font_input_directory = pNewDirectory;
}

void gui::text::initializer::setFontStoreDir( std::string pNewDirectory )
{
	font_store_directory = pNewDirectory;
}

void gui::text::initializer::initFreeType()
{
	if ( FT_Init_FreeType( &ftLib ) ) {
		debug::pushError( "Failed to init FreeType!" );
	}
}

size_t gui::text::initializer::includeFont( std::string pFontFileName, size_t pPointSize, 
        size_t pStartCode, size_t pGlyphCount, int pFlags, size_t pUpsampling )
{
	return includeFont( pFontFileName, 
            FontInstructions( pPointSize, pStartCode, pGlyphCount, pFlags, pUpsampling ) );
}
void removeFileExtension( std::string& pFileName )
{
	size_t nameCharCount = pFileName.size();
	for ( size_t i = 0; i < 4; ++i ) {
		if ( pFileName[nameCharCount - i - 1] == '.' ) {
			nameCharCount = nameCharCount - i - 1;
			break;
		}
	}
	pFileName.resize( nameCharCount );
}
size_t gui::text::initializer::includeFont( std::string pFontFileName, 
        FontInstructions pLoadInstructions = gui::text::FontInstructions() )
{
	size_t sz = loadFontBuffer.size();
	LoadFont font;
	removeFileExtension( pFontFileName );
	font.fileName = pFontFileName;
	font.instructions = pLoadInstructions;
	loadFontBuffer.push_back( font );
	return sz;
}

size_t gui::text::initializer::createFontInstructions( size_t pPointSize, size_t pStartCode, 
        size_t pGlyphCount, int pFlags, size_t pUpsampling )
{
	return createFontInstructions( gui::text::FontInstructions( pPointSize, pStartCode, 
                pGlyphCount, pFlags, pUpsampling ) );
}

size_t gui::text::initializer::createFontInstructions( gui::text::FontInstructions pInstructions )
{
	size_t sz = allFontInstructions.size();
	size_t i = std::find( allFontInstructions.begin(), 
            allFontInstructions.end(), pInstructions ) - allFontInstructions.begin();
	if ( i < sz ) {
		return i;
	}
	allFontInstructions.push_back( pInstructions );
	return sz;
}

//Font file integration
void storeGlyphs( gui::text::Font& pFont, const LoadFont & pLoadFont )
{
	size_t glyCount = pLoadFont.atlas.quads.size();
	std::vector<gui::text::Glyph> glyphs( glyCount );
	pFont.metricOffset = gui::text::allMetrics.size();
	pFont.metricCount = glyCount;
	gui::text::allMetrics.resize( pFont.metricOffset + pFont.metricCount );

	for ( size_t g = 0; g < glyCount; ++g ) {
		const LoadGlyphQuad& qud = pLoadFont.atlas.quads[g];
		const LoadGlyphMetrics& met = pLoadFont.metrics[g];
		glyphs[g] = gui::text::Glyph( ( float )qud.minX / ( float )pLoadFont.atlas.width, ( float )qud.minY / ( float )pLoadFont.atlas.height,
			( float )( qud.maxX ) / ( float )pLoadFont.atlas.width, ( float )( qud.maxY ) / ( float )pLoadFont.atlas.height );
		gui::text::allMetrics[pFont.metricOffset + g] = gui::text::GlyphMetrics( ( float )( qud.maxX - qud.minX ) / ( ( float )gl::Viewport::current->width / 2.0f ), ( float )( qud.maxY - qud.minY ) / ( ( float )gl::Viewport::current->height / 2.0f ),
			( float )met.advanceX / ( ( float )gl::Viewport::current->width / 2.0f ), ( float )met.xBearing / ( ( float )gl::Viewport::current->width / 2.0f ), ( float )met.yBearing / ( ( float )gl::Viewport::current->height / 2.0f ) );
		//allMetrics[pSize.metricOffset + g] = GlyphMetrics( ( float )met.width, ( float )met.height, ( float )met.advanceX, ( float )met.xBearing, ( float )met.yBearing );
	}
	pFont.glyphStorage = gl::Storage<gui::text::Glyph>( "GlyphStorage", 
            glyCount, 0, &glyphs[0] );
	pFont.glyphStorage.setTarget( GL_UNIFORM_BUFFER );
}

std::pair<size_t, size_t> convertKerning( std::vector<int>& pKerningMap )
{
	size_t count = pKerningMap.size();
	size_t offset = gui::text::allKerning.size();
	gui::text::allKerning.resize( offset + count );
	for ( size_t i = 0; i < count; ++i ) {
		gui::text::allKerning[offset + i] = ( float )pKerningMap[i] / ( ( float )gl::Viewport::current->width / 2.0f );
	}
	return std::pair<size_t, size_t>( offset, count );
}

gui::text::Font integrateFont( LoadFont& pFont )
{
	gui::text::Font font;
	glGenTextures( 1, &font.atlasID );
	glBindTexture( GL_TEXTURE_2D, font.atlasID );

	int mipmap_min_flag = GL_NEAREST_MIPMAP_LINEAR;
	int mipmap_mag_flag = GL_NEAREST;

	if ( pFont.instructions.flags & 1 ) {
		mipmap_min_flag = GL_LINEAR_MIPMAP_LINEAR;
		mipmap_mag_flag = GL_LINEAR;
	}
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap_min_flag );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mipmap_mag_flag );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	storeGlyphs( font, pFont );
	std::pair<size_t, size_t> range = convertKerning( pFont.kerningMap );
	font.kerningOffset = range.first;
	font.kerningCount = range.second;
	font.fontMetric.lineGap = ( float )pFont.fontMetrics.lineGap / ( ( float )gl::Viewport::current->height / 2.0f );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, pFont.atlas.width, pFont.atlas.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &pFont.atlas.buffer[0] );

	glGenerateMipmap( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );
	size_t off = gui::text::allFontInstructions.size();
	gui::text::allFontInstructions.push_back( pFont.instructions );
	font.instructions = off;
	return font;
}

void gui::text::initializer::loadFonts()
{

	size_t queueCount = loadFontBuffer.size();
	size_t fontCount = allFonts.size();
	allFonts.resize( fontCount + queueCount );
	for ( size_t f = 0; f < queueCount; ++f ) {
		LoadFont load_font = loadFontBuffer[f];
		loadFont( load_font );
		Font font = integrateFont( load_font );
		allFonts[fontCount + f] = font;
	}
	fontCount += queueCount;
	loadFontBuffer.clear();
}

//TODO: Font Serialization
