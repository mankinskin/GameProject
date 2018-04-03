#include "gui.h"
#include "gldebug.h"
#include "debug.h"
#include "vao.h"
#include "quad.h"
#include "line.h"
#include "quadcolors.h"
#include "text.h"
#include "framebuffer.h"
#include "widget.h"
#include "input.h"
#include "gates.h"
#include "event.h"
#include "functor.h"
#include "mouse.h"
#include "signal.h"
#include "app.h"
#include "lights.h"
#include "viewport.h"
#include "hierarchy.h"
#include "hierarchy_utils.h"

glm::vec2 gui::pixel_size;
using gl::Color;

using utils::Element;
using gui::Quad;
using gl::Color;
using gl::ColorIt;

struct Button 
{
    using Quads = Element<Quad, Quad>;
    using QuadIniter = Element<glm::vec4, glm::vec4>;

    using Colors = Element<gl::ColorIt, gl::ColorIt>;
};

struct Window
{
    struct Frame
    {
        using Quads = utils::gen_Element_t<Quad, 9>; 
        using QuadIniter = utils::gen_Element_t<glm::vec4, 9>;

        using Colors = utils::gen_Element_t<ColorIt, 9>;
        using ColorIniter = utils::gen_Element_t<glm::vec4, 9>;
    };

    struct Header 
    {
        using Quads = Element<Quad, Quad>;
        using QuadIniter = Element<glm::vec4, glm::vec4>;

        using Colors = Element<ColorIt, ColorIt>;
        using ColorIniter = Element<glm::vec4, glm::vec4>;
    };

    using Quads = utils::Element<Frame::Quads, Header::Quads>;
    using QuadIniter = utils::Element<Frame::QuadIniter, Header::QuadIniter>;

    using Colors = utils::Element<Frame::Colors, Header::Colors>;
    using ColorIniter = utils::Element<Frame::ColorIniter, Header::ColorIniter>;
};
template<typename... Qs, typename Vec>
void moveQuads(const Element<Qs...> elem, Vec v)
{
    utils::foreach( gui::moveQuad, elem, v );
}
void gui::init()
{
    pixel_size = glm::vec2( 
            2.0f / gl::getWidth(), 
            2.0f / gl::getHeight() );
    //text::initFonts();
    debug::printErrors();
}

void gui::initWidgets()
{
    using namespace gates;
    using namespace events;
    using namespace functors;
    using namespace input;
    using namespace signals;

    std::string button_border_color_name = "lightgrey";
    Button::Colors buttonColors = Button::Colors( gl::getColor( button_border_color_name ), 
            gl::getColor( "black" ) );

    float button_width = gui::pixel_size.x * 100.0f;
    float button_height = gui::pixel_size.x * 70.0f;
    glm::vec2 margin = gui::pixel_size * 3.0f;

    Button::QuadIniter button_initer( { 
            glm::vec4(0.0f, 0.0f, button_width, button_height ),
            glm::vec4( margin.x, -margin.y, 
                    button_width - margin.x*2.0f, 
                    button_height - margin.y*2.0f ) } );

    std::array<glm::vec2, Button::Quads::COUNT> button_move_policy( { 
            glm::vec2( 1.0f, 1.0f ), 
            glm::vec2( 1.0f, 1.0f ) } );
    //WidgetResizePolicy<Button::Quads> button_resize_policy( { 
    //        glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ), 
    //        glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ) } );

    Button::Quads quitButton_quads( button_initer );
    Button::Quads playButton_quads( button_initer );

    utils::foreach( colorQuad, quitButton_quads, buttonColors );
    utils::foreach( colorQuad, playButton_quads, buttonColors );


    moveQuads( playButton_quads, gui::pixel_round( glm::vec2( -0.9f, -0.6f ) ) );
    moveQuads( quitButton_quads, gui::pixel_round( glm::vec2( -0.9f, -0.8f ) ) );

    ButtonEvents<Event> play_button( 
            createEvent( QuadEvent( playButton_quads.element<1>().index, 1 ) ), 
            createEvent( QuadEvent( playButton_quads.element<1>().index, 0 ) ) );

    gate<and_op, decltype( play_button.hold_evt ), decltype( lmb.on_evt )> play_press_evt( and_op(), 
            play_button.hold_evt, lmb.on_evt );
    ButtonEvents<decltype( play_press_evt ), decltype( lmb.off_evt )> play_lmb( play_press_evt, lmb.off_evt );
    auto move_play_func = 
        createFunctor<void, Button::Quads, glm::vec2&>( moveQuads, playButton_quads, cursorFrameDelta );

    move_play_func.set_triggers( { play_lmb.hold } );


    ButtonEvents<Event> quit_button( 
            createEvent( QuadEvent( quitButton_quads.element<1>().index, 1 ) ), 
            createEvent( QuadEvent( quitButton_quads.element<1>().index, 0 ) ) );

    gate<and_op, decltype( quit_button.hold_evt ), decltype( lmb.on_evt )> quit_press_evt( and_op(), 
            quit_button.hold_evt, lmb.on_evt );
    ButtonEvents<decltype( quit_press_evt ), decltype( lmb.off_evt )> quit_lmb( quit_press_evt, lmb.off_evt );
    auto quit_button_func = 
        createFunctor<void>( app::quit );

    quit_button_func.set_triggers( { quit_lmb.hold } );
    //ButtonEvents<Event> border_btn( 
    //        createEvent( QuadEvent( quitButton.element<0>().index, 1 ) ), 
    //        createEvent( QuadEvent( quitButton.element<0>().index, 0 ) ) );
    //ButtonEvents<Event> center_btn( 
    //        createEvent( QuadEvent( quitButton.element<1>().index, 1 ) ), 
    //        createEvent( QuadEvent( quitButton.element<1>().index, 0 ) ) );

    ////all enter and leave events through or_gates
    //gate<or_op, Event, Event> button_enters_srcs( 
    //        border_btn.on_evt, center_btn.on_evt );
    //gate<or_op, Event, Event> button_leaves_srcs( 
    //        border_btn.off_evt, center_btn.off_evt );

    ////outputs true when an enter occurred and no leave occurred
    //gate<and_op, decltype( button_enters_srcs ), not_gate<decltype( button_leaves_srcs )>> button_enters_src( 
    //        and_op(), button_enters_srcs, not_gate<decltype( button_leaves_srcs )>( button_leaves_srcs ) );
    ////outputs true when a leave occurred and no enter occurred
    //gate<and_op, decltype( button_leaves_srcs ), not_gate<decltype( button_enters_srcs )>> button_leaves_src( 
    //        and_op(), button_leaves_srcs, not_gate<decltype( button_enters_srcs )>( button_enters_srcs ) );
    ////a switch which is turned on by
    //gate<or_op, decltype( button_enters_src ), decltype( button_leaves_src )> ent_or_lea( 
    //        or_op(), button_enters_src, button_leaves_src );
    //toggle_gate<decltype( ent_or_lea )> on_button( ent_or_lea );


    //unsigned int button_enter = createSignal( button_enters_src );
    //unsigned int button_leave = createSignal( button_leaves_src );


    //FunctorRef<void, unsigned int, gl::ColorIt> light_button = 
    //    createFunctor( gui::colorQuad<gl::Color>, quitButton.element<0>().index, gl::getColor( "white" ) );
    //FunctorRef<void, unsigned int, gl::ColorIt> unlight_button = 
    //    createFunctor( gui::colorQuad<gl::Color>, quitButton.element<0>().index, gl::getColor( button_border_color_name ) );
    //light_button.set_triggers( { button_enter } );
    //unlight_button.set_triggers( { button_leave } );

    //gate<and_op, decltype( on_button ), decltype( lmb.on_evt )> button_press( and_op(), on_button, lmb.on_evt );
    //unsigned int quit_button_press = createSignal( button_press );


    //FunctorRef<void> quit_func = createFunctor( app::quit );
    //quit_func.set_triggers( { key_esc.press, quit_button_press } );

    //Window

    Window::Frame::Colors window_frame_colors( { 
            gl::getColor( "grey" ), gl::getColor( "grey" ),  gl::getColor( "grey" ),
            gl::getColor( "grey" ),  gl::getColor( "white" ),  gl::getColor( "grey" ),
            gl::getColor( "grey" ), gl::getColor( "grey" ), gl::getColor( "grey" ) } );

    Window::Header::Colors window_header_colors( { gl::getColor( "grey" ), gl::getColor( "white" ) } );
    Window::Colors window_colors( { window_frame_colors, window_header_colors } );

    float window_width = gui::pixel_size.x * 300.0f;
    float window_height = gui::pixel_size.x * 235.0f;
    glm::vec2 border = gui::pixel_size * 4.0f;

    Window::Frame::QuadIniter window_frame_initer( {
            glm::vec4( 0.0f, 0.0f, border.x, border.y ),
            glm::vec4( border.x, 0.0f, window_width - border.x*2.0f, border.y ),
            glm::vec4( window_width - border.x, 0.0f, border.x, border.y ),
            glm::vec4( 0.0f, -border.y, border.x, window_height - border.y*2.0f ),
            glm::vec4( border.x, -border.y, window_width - border.x*2.0f, window_height - border.y*2.0f ),
            glm::vec4( window_width - border.x, -border.y, border.x, window_height - border.y*2.0f ),
            glm::vec4( 0.0f, -window_height + border.y, border.x, border.y ),
            glm::vec4( border.x, -window_height + border.y, window_width - border.x*2.0f, border.y ),
            glm::vec4( window_width - border.x, -window_height + border.y, border.x, border.y )
            } );

    float header_height = gui::pixel_size.y * 25.0f;
    glm::vec2 header_border = gui::pixel_size * 4.0f;

    Window::Header::QuadIniter window_header_initer( {
            glm::vec4( 0.0f, 0.0f, window_width, header_height ),
            glm::vec4( header_border.x, -header_border.y, window_width - header_border.x*2.0f, header_height - header_border.y*2.0f )
            } );

    Window::QuadIniter window_quad_initer( {  window_frame_initer, window_header_initer } );

    Window::Quads window_quads(window_quad_initer );
    utils::foreach( colorQuad, window_quads, window_colors );

    glm::vec2 mv (-1.0f, 0.0f);

    utils::foreach( moveQuad, window_quads, mv );

    ButtonEvents<Event> header_button( 
            createEvent( QuadEvent( window_quads.element<1>().element<1>().index, 1 ) ), 
            createEvent( QuadEvent( window_quads.element<1>().element<1>().index, 0 ) ) );

    gate<and_op, decltype( header_button.hold_evt ), decltype( lmb.on_evt )> header_press_evt( and_op(), 
            header_button.hold_evt, lmb.on_evt );
    ButtonEvents<decltype( header_press_evt ), decltype( lmb.off_evt )> header_lmb( header_press_evt, lmb.off_evt );
    auto move_window_func = 
        createFunctor<void, Window::Quads, glm::vec2&>( moveQuads, window_quads, cursorFrameDelta );

    move_window_func.set_triggers( { header_lmb.hold } );

    //utils::Element<utils::gen_Element_t<glm::vec2&, 9>, utils::gen_Element_t<glm::vec2&, 2>> el_mv(
    //        utils::gen_Element_t<glm::vec2&, 9>(mv, mv, mv, mv, mv, mv, mv, mv, mv),
    //        utils::gen_Element_t<glm::vec2&, 2>(mv, mv) );
    //std::array<glm::vec2, Window::Frame::Quads::COUNT> window_frame_move_matrix{
    //    glm::vec2( 1.0f, 1.0f ), glm::vec2( 1.0f, 1.0f ), glm::vec2( 1.0f, 1.0f ),
    //        glm::vec2( 1.0f, 1.0f ), glm::vec2( 1.0f, 1.0f ), glm::vec2( 1.0f, 1.0f ),
    //        glm::vec2( 1.0f, 1.0f ), glm::vec2( 1.0f, 1.0f ), glm::vec2( 1.0f, 1.0f )
    //};
    //WidgetResizePolicy<Window::Frame::Quads> window_frame_resize_policy( {
    //        glm::vec4( 0.0f, 0.0f, 0.0f, 0.0f ), glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f ), 
    //        glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f ), glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f ), 
    //        glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ), glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f ),
    //        glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f ), glm::vec4( 0.0f, 1.0f, 1.0f, 0.0f ), 
    //        glm::vec4( 1.0f, 1.0f, 0.0f, 0.0f )
    //        } );

    //std::array<glm::vec2, Window::Header::Quads::COUNT> window_header_move_matrix{
    //    glm::vec2( 1.0f, 1.0f ), glm::vec2( 1.0f, 1.0f )
    //};
    //WidgetResizePolicy<Window::Header> window_header_resize_policy( {
    //        glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ), glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f )
    //        } );

    //std::array<glm::vec2, Window::COUNT> window_move_policy{
    //    glm::vec2( 1.0f, 1.0f ), glm::vec2( 1.0f, 1.0f )
    //};
    //WidgetResizePolicy<Window::Quads> window_resize_policy( {
    //        glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ), glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f )
    //        } );


    //ButtonEvents<Event> right( 
    //        createEvent( QuadEvent( window.element<0>().element<5>().index, 1 ) ), 
    //        createEvent( QuadEvent( window.element<0>().element<5>().index, 0 ) ) );
    //gate<and_op, decltype( right.hold_evt ), decltype( lmb.on_evt )> 
    //    right_press_evt( and_op(), right.hold_evt, lmb.on_evt );
    //ButtonEvents<decltype( right_press_evt ), decltype( lmb.off_evt )> right_and_lmb( 
    //        right_press_evt, lmb.off_evt );

    //ButtonEvents<Event> bottom( 
    //        createEvent( QuadEvent( window.element<0>().element<7>().index, 1 ) ), 
    //        createEvent( QuadEvent( window.element<0>().element<7>().index, 0 ) ) );
    //gate<and_op, decltype( bottom.hold_evt ), decltype( lmb.on_evt )> bottom_press_evt( and_op(), 
    //        bottom.hold_evt, lmb.on_evt );
    //ButtonEvents<decltype( bottom_press_evt ), decltype( lmb.off_evt )> bottom_and_lmb( 
    //        bottom_press_evt, lmb.off_evt );

    //ButtonEvents<Event> bottom_right( createEvent( QuadEvent( window.element<0>().element<8>().index, 1 ) ), createEvent( QuadEvent( window.element<0>().element<8>().index, 0 ) ) );
    //gate<and_op, decltype( bottom_right.hold_evt ), decltype( lmb.on_evt )> bottom_right_press_evt( and_op(), bottom_right.hold_evt, lmb.on_evt );
    //ButtonEvents<decltype( bottom_right_press_evt ), decltype( lmb.off_evt )> bottom_right_and_lmb( bottom_right_press_evt, lmb.off_evt );


    //FunctorRef<void, Window, glm::vec2&> resize_window_func = 
    //    createFunctor<void, Window, glm::vec2&>( resize_widget<Window>, window, cursorFrameDelta );
    //resize_window_func.set_triggers( { bottom_right_and_lmb.hold } );

    //FunctorRef<void, Window, float&, float> resize_window_x_func = createFunctor<void, Window, float&, float>( resize_widget, window, cursorFrameDelta.x, 0.0f );
    //resize_window_x_func.set_triggers( { right_and_lmb.hold } );

    //FunctorRef<void, Window, float, float&> resize_window_y_func = createFunctor<void, Window, float, float&>( resize_widget, window, 0.0f, cursorFrameDelta.y );
    //resize_window_y_func.set_triggers( { bottom_and_lmb.hold } );

    ////Slider
    //using Slider = QuadGroup<2>;
    //using SliderResizePolicy = WidgetResizePolicy<Slider>;
    //using SliderColors = ColorGroup<2, gl::ColorIt>;

    //SliderColors slider_colors( gl::getColor( "lightgrey" ), gl::getColor( "darkgrey" ) );
    //Slider::initer_t slider_initer( { glm::vec4( 0.0f, 0.0f, 0.2f, 0.05f ), glm::vec4( 0.0f, 0.0f, 0.05f, 0.05f ) } );
    //std::array<glm::vec2, Slider::ELEMENT_COUNT> slider_move_matrix{ glm::vec2( 1.0f, 1.0f ), glm::vec2( 1.0f, 1.0f ) };
    //SliderResizePolicy slider_resize_policy( { glm::vec4( 0.0f, 0.0f,1.0f, 1.0f ), glm::vec4( 0.0f, 0.0f,1.0f, 1.0f ) } );

    //Slider slider( slider_initer );
    //slider.color( slider_colors );
    //slider.move( 0.7f, -0.7f );


    //Event slider_box_enter_evt = createEvent( QuadEvent( slider.element<0>().index, 1 ) );
    //unsigned int slider_box_enter = createSignal( slider_box_enter_evt );
    //Event slider_box_leave_evt = createEvent( QuadEvent( slider.element<0>().index, 0 ) );
    //unsigned int slider_box_leave = createSignal( slider_box_leave_evt );

    //Event slider_slide_enter_evt = createEvent( QuadEvent( slider.element<1>().index, 1 ) );
    //unsigned int slider_slide_enter = createSignal( slider_slide_enter_evt );
    //Event slider_slide_leave_evt = createEvent( QuadEvent( slider.element<1>().index, 0 ) );
    //unsigned int slider_slide_leave = createSignal( slider_slide_leave_evt );


    //gate<or_op, Event, Event> slider_any_enter_evt( or_op(), slider_slide_enter_evt, slider_box_enter_evt );
    //gate<or_op, Event, Event> slider_any_leave_evt( or_op(), slider_slide_leave_evt, slider_box_leave_evt );

    //gate<and_op, decltype( slider_any_enter_evt ), not_gate<decltype( slider_any_leave_evt )>> slider_enter_evt( and_op(), slider_any_enter_evt, not_gate<decltype( slider_any_leave_evt )>( slider_any_leave_evt ) );
    //gate<and_op, decltype( slider_any_leave_evt ), not_gate<decltype( slider_any_enter_evt )>> slider_leave_evt( and_op(), slider_any_leave_evt, not_gate<decltype( slider_any_enter_evt )>( slider_any_enter_evt ) );
    //switch_gate<decltype( slider_enter_evt ), decltype( slider_leave_evt )> slider_hover_evt( slider_enter_evt, slider_leave_evt );

    //gate<and_op, decltype( slider_hover_evt ), decltype( lmb.on_evt )> slider_press_evt( and_op(), slider_hover_evt, lmb.on_evt );

    //ButtonEvents<decltype( slider_press_evt ), Event> slider_lmb( slider_press_evt, lmb.off_evt );

    //void( *move_quad_to )( Quad, float& ) = []( Quad pTar, float& pPos )->void {
    //    float dist = pPos - ( pTar.get_pos().x + pTar.get_size().x / 2.0f );
    //    pTar.move( glm::vec2( dist, 0.0f ) );
    //};

    //FunctorRef<void, Quad, float&> move_slide_func = createFunctor<void, Quad, float&>( move_quad_to, slider.element<1>().index, relativeCursorPosition.x );
    //move_slide_func.set_triggers( { slider_lmb.hold } );

    //void( *lim_quad )( Quad, Quad ) = []( Quad pTar, Quad pLim )->void {
    //    float l_dist = glm::vec2( pLim.get_pos() - pTar.get_pos() ).x;
    //    float r_dist = glm::vec2( ( pLim.get_pos() + pLim.get_size() ) - ( pTar.get_pos() + pTar.get_size() ) ).x;
    //    pTar.move( glm::vec2( std::min( std::max( l_dist, 0.0f ), r_dist ), 0.0f ) );
    //};

    //FunctorRef<void, Quad, Quad> limit_slide_func = createFunctor( lim_quad, slider.element<1>(), slider.element<0>() );
    //limit_slide_func.set_triggers( { slider_lmb.hold } );

    //void( *set_slide_target )( glm::vec4&, Quad, Quad ) = []( glm::vec4& pTarget, Quad pBox, Quad pSlide )->void {
    //    float half_width = pSlide.get_size().x / 2.0f;
    //    float slide_pos = pSlide.get_pos().x + half_width;
    //    float box_size = pBox.get_size().x;
    //    float box_pos = pBox.get_pos().x;
    //    float amt = ( slide_pos - box_pos ) / ( box_size - half_width );
    //    pTarget = glm::vec4( amt, amt, amt, amt )*10.0f;
    //};
    //FunctorRef<void, glm::vec4&, Quad, Quad> set_slide_target_func = createFunctor<void, glm::vec4&, Quad, Quad>( set_slide_target, lights::getLightColor( 0 ), slider.element<0>(), slider.element<1>() );
    //set_slide_target_func.set_triggers( { slider_lmb.hold } );
    //
    ////text
    //gui::text::createTextboxMetrics( 0, 1.0, 1.0, 1.0, 1.0 );
    //
    //unsigned int qu_tb = gui::text::createTextbox( quitButton.element<1>().index, 0, TEXT_LAYOUT_CENTER_Y );
    ////unsigned int fps_tb = gui::text::createTextbox( fps_box.element<0>().element(), 0, TEXT_LAYOUT_CENTER_Y );
    //
    //gui::text::setTextboxString( qu_tb, " QUIT" );
    ////gui::text::setTextboxString( pl_tb, " Play" );
    ////gui::text::setTextboxString( fps_tb, "FPS" );
    //gui::text::loadTextboxes();
}

glm::vec2 gui::pixel_round( glm::vec2 pIn ) {
    return pixel_size * round( pIn / pixel_size );
}
float gui::pixel_round_x( float pIn ) {
    return pixel_size.x * round( pIn / pixel_size.x );
}
float gui::pixel_round_y( float pIn ) {
    return pixel_size.y * round( pIn / pixel_size.y );
}
