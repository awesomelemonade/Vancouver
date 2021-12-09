#define GLM_ENABLE_EXPERIMENTAL
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

#include <eigen3/Eigen/Dense>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

using namespace Leap;
using namespace std;

class LeapListener: public Listener {
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
		virtual int getNumFingersExtended();
		virtual int getMovement();

		float handYaw = 0.0f;
		float handRoll = 0.0f;
		int numHands = 0;
		int numFingersExtended = 0;
		float indexFingerVelocity = 0.0f;
		deque<float> velocityQueue;
		float velocitySum = 0.0f;
		int averageSize = 8;
		bool thumbExtended = false;
		bool othersExtended = false;
		Vector prevHandPosition;
		int movement = 0;

	private:
};

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

int LeapListener::getNumFingersExtended() {
	return !thumbExtended ? -1 : (!othersExtended ? 1 : 0);
	//return numFingersExtended;
}

int LeapListener::getMovement() {
	return movement;
}

void LeapListener::onInit(const Controller& controller) {
	std::cout << "Initialized" << std::endl;
}

void LeapListener::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;
	controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_SWIPE);
}

void LeapListener::onDisconnect(const Controller& controller) {
	// Note: not dispatched when running in a debugger.
	std::cout << "Disconnected" << std::endl;
}

void LeapListener::onExit(const Controller& controller) {
	std::cout << "Exited" << std::endl;
}

static map<pair<Finger::Type, Bone::Type>, glm::vec3> boneDirections;

void LeapListener::onFrame(const Controller& controller) {
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
		const auto handPosition = hand.palmPosition();
		const Vector normal = hand.palmNormal();
		const Vector direction = hand.direction();
		auto leapHandVelocity = handPosition - prevHandPosition;
		auto handVelocity = glm::vec3(leapHandVelocity.x, leapHandVelocity.y, leapHandVelocity.z);
		prevHandPosition = handPosition;
		//std::cout << handVelocity.x << "\n";

		// Calculate the hand's pitch, roll, and yaw angles
		/*std::cout << std::string(2, ' ') <<  "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
		  << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
		  << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;*/

		handYaw = direction.yaw();
		handRoll = normal.roll();

		// Get fingers
		auto totalLength = handVelocity.x;
		const FingerList fingers = hand.fingers().extended();
		thumbExtended = false;
		auto othersCount = 0;
		for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
			const Finger finger = *fl;
			auto fingerName = fingerNames[finger.type()];
			if (fingerName == "Thumb") {
				thumbExtended = true;
			} else {
				othersCount++;
			}
		}
		othersExtended = othersCount == 4;
		//cout << totalLength << "\n" << std::flush;
		velocityQueue.push_back(totalLength);
		velocitySum += totalLength;
		while (velocityQueue.size() > averageSize) {
			velocitySum -= velocityQueue.front();
			velocityQueue.pop_front();
		}
		auto averageHandVelocity = velocitySum / velocityQueue.size();
		if (averageHandVelocity > 1) {
			movement = 1;
		} else if (averageHandVelocity < -1) {
			movement = -1;
		} else {
			movement = 0;
		}


		//indexFingerVelocity = totalLength;
	}
	std::cout << std::flush;
}

void LeapListener::onFocusGained(const Controller& controller) {
	std::cout << "Focus Gained" << std::endl;
}

void LeapListener::onFocusLost(const Controller& controller) {
	std::cout << "Focus Lost" << std::endl;
}

void LeapListener::onDeviceChange(const Controller& controller) {
	std::cout << "Device Changed" << std::endl;
	const DeviceList devices = controller.devices();

	for (int i = 0; i < devices.count(); ++i) {
		std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
	}
}

void LeapListener::onServiceConnect(const Controller& controller) {
	std::cout << "Service Connected" << std::endl;
}

void LeapListener::onServiceDisconnect(const Controller& controller) {
	std::cout << "Service Disconnected" << std::endl;
}
