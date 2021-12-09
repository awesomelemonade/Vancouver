/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include <cstring>
#include "Leap.h"

#include <map>
#include <utility>
#include <deque>

using namespace Leap;
using namespace std;

class SampleListener : public Listener {
  public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);

    float handYaw = 0.0f;
    float handRoll = 0.0f;
    int numHands = 0;
    int numFingersExtended = 0;
    float indexFingerVelocity = 0.0f;
    deque<float> velocityQueue;
    float velocitySum = 0.0f;
    int averageSize = 100;

  private:
};

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

void SampleListener::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
}

static map<pair<Finger::Type, Bone::Type>, glm::vec3> boneDirections;

void SampleListener::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  //std::cout << "Frame id: " << frame.id()
  //          << ", timestamp: " << frame.timestamp()
  //          << ", hands: " << frame.hands().count()
  //          << ", extended fingers: " << frame.fingers().extended().count()
  //          << ", tools: " << frame.tools().count()
  //          << ", gestures: " << frame.gestures().count() << std::endl;

  numHands = frame.hands().count();
  numFingersExtended = frame.fingers().extended().count();
  HandList hands = frame.hands();
  for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    // Get the first hand
    const Hand hand = *hl;
    std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
    //std::cout << std::string(2, ' ') << handType << ", id: " << hand.id()
    //          << ", palm position: " << hand.palmPosition() << std::endl;
    // Get the hand's normal vector and direction
    const Vector normal = hand.palmNormal();
    const Vector direction = hand.direction();

    // Calculate the hand's pitch, roll, and yaw angles
    /*std::cout << std::string(2, ' ') <<  "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
              << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
              << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;*/

    handYaw = direction.yaw();
    handRoll = normal.roll();

    // Get fingers
	auto totalLength = 0.0f;
    const FingerList fingers = hand.fingers();
    for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
      const Finger finger = *fl;
      auto fingerName = fingerNames[finger.type()];

      if (fingerName == "Index" || fingerName == "Middle") {
	      // TODO
	      /*std::cout << std::string(4, ' ') <<  fingerNames[finger.type()]
			<< " finger, id: " << finger.id()
			<< ", length: " << finger.length()
			<< "mm, width: " << finger.width() << std::endl;*/
	      

	      // Get finger bones
	      for (int b = 0; b < 4; ++b) {
		Bone::Type boneType = static_cast<Bone::Type>(b);
		Bone bone = finger.bone(boneType);
		/*std::cout << std::string(6, ' ') <<  boneNames[boneType]
			  << " bone, start: " << bone.prevJoint()
			  << ", end: " << bone.nextJoint()
			  << ", direction: " << bone.direction() << std::endl;*/
		auto boneDirection = bone.direction();
		auto key = make_pair(finger.type(), boneType);
		auto value = glm::vec3(boneDirection.x, boneDirection.y, boneDirection.z);
		auto oldBoneDirection = boneDirections[key];

		if (boneDirections.count(key)) {
			auto diff = oldBoneDirection - value;
			auto length = glm::length(diff);
			totalLength += length;
			//cout << "LENGTH: " << length << "\n" << std::flush;
		}

		boneDirections[key] = value;
	      }
      }
    }
	//cout << totalLength << "\n" << std::flush;
	velocityQueue.push_back(totalLength);
	velocitySum += totalLength;
	while (velocityQueue.size() > averageSize) {
	      velocitySum -= velocityQueue.front();
	      velocityQueue.pop_front();
	}
	indexFingerVelocity = velocitySum / velocityQueue.size();
	//indexFingerVelocity = totalLength;
  }
  std::cout << std::flush;
}

void SampleListener::onFocusGained(const Controller& controller) {
  std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
  std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
  std::cout << "Device Changed" << std::endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
	  std::cout << "id: " << devices[i].toString() << std::endl;
	  std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
  }
}

void SampleListener::onServiceConnect(const Controller& controller) {
  std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
  std::cout << "Service Disconnected" << std::endl;
}
