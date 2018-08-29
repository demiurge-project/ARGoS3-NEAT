/**
  * @file <loop-functions/ColorsLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "ColorsLoopFunc.h"

/****************************************/
/****************************************/

ColorsLoopFunction::ColorsLoopFunction() {
  m_fRadius = 0.40;
  m_cCoordSpot1 = CVector2(0,1.15);
  m_cCoordSpot2 = CVector2(0,-1.15);
  m_fObjectiveFunction = 0;
  m_bInitializationStep = true;
}

/****************************************/
/****************************************/

ColorsLoopFunction::ColorsLoopFunction(const ColorsLoopFunction& orig) {
}

/****************************************/
/****************************************/

ColorsLoopFunction::~ColorsLoopFunction() {}

/****************************************/
/****************************************/

void ColorsLoopFunction::Destroy() {
}

/****************************************/
/****************************************/

argos::CColor ColorsLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {
  CVector2 vCurrentPoint(c_position_on_plane.GetX(), c_position_on_plane.GetY());

  if (vCurrentPoint.GetY() > 0.76)
      return CColor::BLACK;
  else if (vCurrentPoint.GetY() < -0.76)
      return CColor::WHITE;
  else
      return CColor::GRAY50;
}


/****************************************/
/****************************************/

void ColorsLoopFunction::Reset() {
  m_fObjectiveFunction = 0;
  for (UInt32 i = 0; i <= m_unNumberRobots; i++) {
    m_punFoodData[i] = 0;
  }
  AutoMoDeLoopFunctions::Reset();
}

/****************************************/
/****************************************/

void ColorsLoopFunction::PostStep() {
    ArenaControl();
    if (m_bInitializationStep) {
      m_punFoodData = new UInt32[m_unNumberRobots+1];
      for (UInt32 i = 0; i <= m_unNumberRobots; i++) {
        m_punFoodData[i] = 0;
      }
      m_bInitializationStep = false;
    }
    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    UInt32 unId;
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
      CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
      unId = atoi(pcEpuck->GetId().substr(5, 6).c_str());
      cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                         pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

      if (cEpuckPosition.GetY() >= 0.76) {
          m_punFoodData[unId] = 1;
      }
      else if (cEpuckPosition.GetY() <= -0.76) {
          m_fObjectiveFunction = m_fObjectiveFunction + m_punFoodData[unId];
          m_punFoodData[unId] = 0;
      }
    }
    LOG << "Score: " << m_fObjectiveFunction << std::endl;
}

/****************************************/
/****************************************/

Real ColorsLoopFunction::GetObjectiveFunction() {
  return m_fObjectiveFunction;
}

/****************************************/
/****************************************/

void ColorsLoopFunction::ArenaControl() {
    UInt32 unClock = GetSpace().GetSimulationClock();
    switch (unClock){

    case 200:
        for (UInt32 i=1;i<=4;i++)
        {
            m_pcArena->SetBoxColor(i,1,CColor::GREEN);
            m_pcArena->SetBoxColor(i,4,CColor::BLUE);
        }
        for (UInt32 i=3;i<=6;i++)
        {
            m_pcArena->SetBoxColor(i,2,CColor::GREEN);
            m_pcArena->SetBoxColor(i,5,CColor::BLUE);
        }
        break;

    case 1000:
        m_pcArena->SetArenaColor(CColor::RED);
        break;

    case 1200:
        m_pcArena->SetArenaColor(CColor::GRAY60);
        for (UInt32 i=1;i<=4;i++)
        {
            m_pcArena->SetBoxColor(i,1,CColor::GREEN);
            m_pcArena->SetBoxColor(i,4,CColor::BLUE);
        }
        for (UInt32 i=3;i<=6;i++)
        {
            m_pcArena->SetBoxColor(i,2,CColor::GREEN);
            m_pcArena->SetBoxColor(i,5,CColor::BLUE);
        }
        break;

    case 2000:
        m_pcArena->SetArenaColor(CColor::RED);
        break;

    case 2200:
        m_pcArena->SetArenaColor(CColor::GRAY60);
        for (UInt32 i=1;i<=4;i++)
        {
            m_pcArena->SetBoxColor(i,1,CColor::GREEN);
            m_pcArena->SetBoxColor(i,4,CColor::BLUE);
        }
        for (UInt32 i=3;i<=6;i++)
        {
            m_pcArena->SetBoxColor(i,2,CColor::GREEN);
            m_pcArena->SetBoxColor(i,5,CColor::BLUE);
        }
        break;

    case 3000:
        m_pcArena->SetArenaColor(CColor::RED);
        break;

    case 3200:
        m_pcArena->SetArenaColor(CColor::GRAY60);
        for (UInt32 i=1;i<=4;i++)
        {
            m_pcArena->SetBoxColor(i,1,CColor::GREEN);
            m_pcArena->SetBoxColor(i,4,CColor::BLUE);
        }
        for (UInt32 i=3;i<=6;i++)
        {
            m_pcArena->SetBoxColor(i,2,CColor::GREEN);
            m_pcArena->SetBoxColor(i,5,CColor::BLUE);
        }
        break;

    default:
        break;
    }
    return;
}

/****************************************/
/****************************************/

CVector3 ColorsLoopFunction::GetRandomPosition() {
  Real temp;
  Real a = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  Real b = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  // If b < a, swap them
  if (b < a) {
    temp = a;
    a = b;
    b = temp;
  }
  Real fPosX = b * m_fDistributionRadius * cos(2 * CRadians::PI.GetValue() * (a/b));
  Real fPosY = b * m_fDistributionRadius * sin(2 * CRadians::PI.GetValue() * (a/b));

  return CVector3(fPosX, fPosY, 0);
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(ColorsLoopFunction, "colors_loop_function");
