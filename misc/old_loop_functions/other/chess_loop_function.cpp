#include "chess_loop_function.h"

/****************************************/
/****************************************/

CChessLoopFunctions::CChessLoopFunctions() :
   CNeatLoopFunctions() {}

/****************************************/
/****************************************/

CChessLoopFunctions::~CChessLoopFunctions() {
}

/****************************************/
/****************************************/

CColor CChessLoopFunctions::GetFloorColor(const CVector2& c_pos_on_floor) {
   
   Real l = 1.5;
   Real rX = c_pos_on_floor.GetX() - l*floor(c_pos_on_floor.GetX()/l);
   Real rY = c_pos_on_floor.GetY() - l*floor(c_pos_on_floor.GetY()/l);

   /*if(rY>=0 && rY<l/3) {
      if(rX>=0 && rX<l/3) {
	 return CColor::RED;
      } else if(rX>=l/3 && rX<2*l/3) {
	 return CColor::GREEN;
      } else {
	 return CColor::BLUE;
      }
   } else if(rY>=l/3 && rY<2*l/3) {
      if(rX>=0 && rX<l/3) {
	 return CColor::GREEN;
      } else if(rX>=l/3 && rX<2*l/3) {
	 return CColor::BLUE;
      } else {
	 return CColor::RED;
      }
   } else {
      if(rX>=0 && rX<l/3) {
	 return CColor::BLUE;
      } else if(rX>=l/3 && rX<2*l/3) {
	 return CColor::RED;
      } else {
	 return CColor::GREEN;
      }
   }*/

   if(rY>=0 && rY<l/2) {
      if(rX>=0 && rX<l/2) {
	 return CColor::BLACK;
      } else {
	 return CColor::WHITE;
      }
   } else {
      if(rX>=0 && rX<l/2) {
	 return CColor::WHITE;
      } else {
	 return CColor::BLACK;
      }
   }
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CChessLoopFunctions, "chess_loop_functions")
