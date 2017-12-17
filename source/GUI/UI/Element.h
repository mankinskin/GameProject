#pragma once
#include "Quad.h"
#include <tuple>
#include "Colorings.h"
#include <glm\glm.hpp>

namespace gui {

	const float aspect_ratio = 1600.0f / 850.0f;

	//---Widget Initializers
	//- can be passed to Widget constructor, which will invoke the 
	template<typename Wid, template<typename, typename...> typename Policy, typename... paramArgs>
	struct Initializer : Policy<Wid, paramArgs...> {
		template<class... Args>
		Initializer(Args... pArgs)
			:Policy<Wid, paramArgs...>({ pArgs }...) {}
	};


	//Initializer Policies
	//used for Policy template template parameter
	//needs an overload for the `Element` conversion operator
	//you can freely add new functions and use them in calling code or overload Initializer's 
	template<class Wid>
	struct initSimple {
		explicit initSimple(float pPosX, float pPosY, float pWidth, float pHeight)
			:posX(pPosX), posY(pPosY), width(pWidth), height(pHeight) {}
		float posX;
		float posY;
		float width;
		float height;

		operator Wid() const {
			return Wid();
		}
	};

	template<class Wid>
	struct initMargin {
		explicit initMargin(float pPosX, float pPosY, float pWidth, float pHeight, float pMargin)
			:posX(pPosX), posY(pPosY), width(pWidth), height(pHeight), margin(pMargin) {}
		float posX;
		float posY;
		float width;
		float height;
		float margin;

		operator Wid() const {
			return Wid();
		}
	};

	template<class Wid, class ...Initializers>
	struct initJoined {
		explicit initJoined(Initializers... pInits)
			:initializers{ Initializers(pInits)...} {}
		explicit initJoined(std::tuple<Initializers...> pInits)
			:initializers{ pInits } {}
		std::tuple<Initializers...>  initializers;

		
		operator Wid() const {
			return Wid(initializers);
		}
	};


}
