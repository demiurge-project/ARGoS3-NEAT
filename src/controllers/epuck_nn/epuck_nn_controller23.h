#ifndef EPUCK_NN_CONTROLLER
#define EPUCK_NN_CONTROLLER


#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/vector2.h>

#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_wheels_actuator.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_range_and_bearing_actuator.h>

#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_proximity_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_light_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_ground_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_range_and_bearing_sensor.h>

#include "../../NEAT/network.h"
#include "../../NEAT/genome.h"

#include <map>

using namespace argos;

class CEPuckNNController : public CCI_Controller {

public:

   CEPuckNNController();
   virtual ~CEPuckNNController();

   void Init(TConfigurationNode& t_node);
   void ControlStep();
   void Reset();
   void Destroy();

   void LoadNetwork(const std::string& filename);

   void Display(int i);
   void DisplayNetwork();

   inline void SetNetwork(NEAT::Network& net) {
      m_net = &net;
   }

   UInt32 getRobotId();

private:

   // Actuators
   CCI_EPuckWheelsActuator* m_pcWheels;
   CCI_EPuckRangeAndBearingActuator* m_pcRABAct;

   // Sensors
   CCI_EPuckProximitySensor* m_pcProximity;
   CCI_EPuckLightSensor* m_pcLight;
   CCI_EPuckGroundSensor* m_pcGround;
   CCI_EPuckRangeAndBearingSensor* m_pcRAB;

   // Network
   NEAT::Network* m_net;

   // Path to the genome
   std::string m_strFile;

   // Range And Bearing Packet (which contains the id, timestamp, and the real packet which contains the range, bearing and the data)
   struct RBPacket {
         UInt32 unId;
         UInt32 unTimestamp;
         CCI_EPuckRangeAndBearingSensor::SReceivedPacket cMessage;

         RBPacket() {
         };

         RBPacket(UInt32 id, UInt32 timestamp, CCI_EPuckRangeAndBearingSensor::SReceivedPacket message) :
            unId(id),
            unTimestamp(timestamp),
            cMessage(message) {
         }
   };

   SInt32 m_nId;
   std::map<UInt32, RBPacket> m_mapMessages;
   UInt32 m_unTimeStep;
   UInt32 m_unRABTime;

   Real m_inputs[23];
   Real m_fLeftSpeed, m_fRightSpeed;
   bool m_bTrial;
};

#endif
