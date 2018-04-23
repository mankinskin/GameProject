#pragma once
#include <gl\glew.h>
#include <glm/glm.hpp>
#include  <string>
#include <GLFW\glfw3.h>
namespace camera {
	class Camera;
	struct LookPolicy {//stores relative offsets of vectors in a camera object to be used to move the camera frustum
		LookPolicy( size_t pXRotatorOffset, size_t pXRotationAxisOffset, float pXRotationFactor,
			size_t pYRotatorOffset, size_t pYRotationAxisOffset, float pYRotationFactor, size_t pCrossUpOffset, glm::vec3 pAxisLimits, float pSensitivity = 0.5f )
			:x_rotator_off( pXRotatorOffset ), x_rotation_axis_off( pXRotationAxisOffset ), x_rotation_factor( pXRotationFactor ),
			y_rotator_off( pYRotatorOffset ), y_rotation_axis_off( pYRotationAxisOffset ), y_rotation_factor( pYRotationFactor ), cross_up_off( pCrossUpOffset ), axis_limits( pAxisLimits ), sensitivity( pSensitivity ){}
		
		void apply( Camera* pCam ) {
			x_rotator_off += ( size_t )pCam;
			x_rotation_axis_off += ( size_t )pCam;
			y_rotator_off += ( size_t )pCam;
			y_rotation_axis_off += ( size_t )pCam;
			cross_up_off += ( size_t )pCam;
		}
		size_t x_rotator_off;
		size_t x_rotation_axis_off;
		float x_rotation_factor;
		size_t y_rotator_off;
		size_t y_rotation_axis_off;
		float y_rotation_factor;
		size_t cross_up_off;
		glm::vec3 axis_limits;
		float sensitivity;
	};

	struct MovementPolicy {//stores relative offsets of vectors in a camera to be used to move the camera
		MovementPolicy( 
			size_t pForwardOffset, float pForwardFactor,
			size_t pRightOffset, float pRightFactor,
			size_t pUpwardOffset, float pUpwardFactor, float pSpeed = 0.5f )
			:forward_off( pForwardOffset ), forward_factor( pForwardFactor ),
			right_off( pRightOffset ), right_factor( pRightFactor ),
			upward_off( pUpwardOffset ), upward_factor( pUpwardFactor ), speed( pSpeed ){}
		void apply( Camera* pCam ) {
			forward_off += ( size_t )pCam;
			right_off += ( size_t )pCam;
			upward_off += ( size_t )pCam;
		}
		size_t forward_off;
		float forward_factor;
		size_t right_off;
		float right_factor;
		size_t upward_off;
		float upward_factor;
		float speed;
	};
	struct Frustum {
		Frustum( glm::vec3 pUp, glm::vec3 pCenter, float pFOV, float pWidth, float pHeight, float pNearPlane, float pFarPlane )
			:up( pUp ), center( pCenter ), fov( pFOV ), width( pWidth ), height( pHeight ), nearPlane( pNearPlane ), farPlane( pFarPlane ){}
		glm::vec3 up;
		glm::vec3 center;
		float fov = 70.0f;
		float width = 1000.0f;
		float height = 650.0f;
		float nearPlane = 1.0f;
		float farPlane = 1000.0f;
	};

	struct CameraMode {
		CameraMode( size_t pModeID, Frustum pFrustum, MovementPolicy pMovement, LookPolicy pLook )
			:id( pModeID ), frustum( pFrustum ), movement( pMovement ), look( pLook ) {}
		void apply( Camera* pCam );
		Frustum frustum;
		MovementPolicy movement;
		LookPolicy look;
		size_t id;
	};
	

	struct CameraControls {
		CameraControls( int pmove_forward_key, int pmove_backward_key, int pmove_left_key,
			int pmove_right_key, int pmove_up_key, int pmove_down_key ):
			move_forward_key( pmove_forward_key ), move_backward_key( pmove_backward_key ),
			move_left_key( pmove_left_key ), move_right_key( pmove_right_key ),
			move_up_key( pmove_up_key ), move_down_key( pmove_down_key ){}
		int move_forward_key;
		int move_backward_key;
		int move_left_key;
		int move_right_key;
		int move_up_key;
		int move_down_key;
		int toggle_look_key;
	};

	const CameraControls DEFAULT_CAMERA_CONTROLS = CameraControls( GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_Y );

	class Camera { 
	public:
		Camera( CameraMode pMode, CameraControls pControls = DEFAULT_CAMERA_CONTROLS )
			:mode( pMode ), controls( pControls ){}
		void init();
		glm::vec3& getPos();
		glm::mat4& getProjection();
		glm::mat4& getView();
		void setPosition( glm::vec3 pPos );
		void translateLocal( glm::vec3 pDir );
		void translateGlobal( glm::vec3 pDir );
		void translateLocal( float pX, float pY, float pZ );
		void look( glm::vec2 pDir );
		void lookAtCenter();
		void move( glm::vec3 pDir );
		void toggleLook();
		void forward();
		void back();
		void left();
		void right();
		void up();
		void down();
		void update();
		void setMode( CameraMode pMode );
		void nextMode();
		void setSpeed( float pNewSpeed );
		void setSensitivity( float pNewSensitivity );
		glm::vec3 mov;
		bool followCursor = false;
		glm::vec3 pos = glm::vec3();
		glm::vec3 lookAt = glm::vec3( 0.0f, 0.0f, -1.0f );
		glm::vec3 normal;
		glm::vec3 cross;
		glm::mat4 viewMatrix = {};
		glm::mat4 projectionMatrix = {};
		glm::mat4 infiniteProjectionMatrix = {};
		glm::mat4 orthoProjectionMatrix = {};
		std::string name = "camera";
		CameraMode mode;
		CameraControls controls;

	};

	const MovementPolicy TOP_DOWN_MOVEMENT = MovementPolicy( offsetof( Camera, normal ), 1.0f, offsetof( Camera, cross ), 1.0f, offsetof( Camera, lookAt ), -1.0f );
	const Frustum TOP_DOWN_FRUSTUM = Frustum( glm::vec3( 0.0, 0.0, -1.0 ), glm::vec3( 0.0, -1.0, 0.0 ), 70.0f, 1600.0f, 850.0f, 1.0f, 1000.0f );
	const LookPolicy TOP_DOWN_LOOK = LookPolicy( offsetof( Camera, normal ), offsetof( Camera, lookAt ), -1.0f, offsetof( Camera, normal ), offsetof( Camera, lookAt ), 0.0f, offsetof( Camera, normal ), glm::vec3( 0.0f, 0.0f, 0.0f ) );

	const MovementPolicy FIRST_PERSON_MOVEMENT = MovementPolicy( offsetof( Camera, lookAt ), 1.0f, offsetof( Camera, cross ), 1.0f, offsetof( Camera, normal ), 1.0f );
	const Frustum FIRST_PERSON_FRUSTUM = Frustum( glm::vec3( 0.0, 1.0, 0.0 ), glm::vec3( 0.0, 0.0, -1.0 ), 70.0f, 1600.0f, 850.0f, 1.0f, 1000.0f );
	const LookPolicy FIRST_PERSON_LOOK = LookPolicy( offsetof( Camera, lookAt ), offsetof( Camera, mode ) + offsetof( CameraMode, frustum ) + offsetof( Frustum, up ), 1.0f, offsetof( Camera, lookAt ), offsetof( Camera, cross ), 1.0f, offsetof( Camera, mode ) + offsetof( CameraMode, frustum ) + offsetof( Frustum, up ), glm::vec3( 0.0f, 0.0f, 0.0f ), 1.0f );
	const size_t CAMERA_MODE_COUNT = 2;
	const CameraMode CAMERA_MODES[CAMERA_MODE_COUNT] = { CameraMode( 0, FIRST_PERSON_FRUSTUM, FIRST_PERSON_MOVEMENT, FIRST_PERSON_LOOK ) , CameraMode( 1, TOP_DOWN_FRUSTUM, TOP_DOWN_MOVEMENT, TOP_DOWN_LOOK ) };
	extern Camera main_camera;

	void forward( Camera& pCam );
	void back( Camera& pCam );
	void left( Camera& pCam );
	void right( Camera& pCam );
	void up( Camera& pCam );
	void down( Camera& pCam );
	void toggleLook( Camera& pCam );
	void setMode( Camera& pCam, CameraMode pMode );
	void cycleModes( Camera& pCam );
	void setSpeed( Camera& pCam, float pSpeed );
	void setSensitivity( Camera& pCam, float pSpeed );
};
