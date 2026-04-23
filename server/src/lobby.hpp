#pragma once

#include "net/msg.hpp"
#include <App.h>
#include <memory>
#include <unordered_map>
#include <vector>

struct Player {
  uWS::WebSocket<false, true, std::string> *ws;
  int id;
};

struct Car {
  float x, y = 0;
  float rotation = 0;

  float throttle = 0;
  float steering = 0;

  float velocity = 0;
  float acceleration = 100;
  float maxVelocity = 150;
  float friction = 50;

  float turnSpeed = 20;

  float tireRotation = 0;
  float tireTurnSpeed = 70;
  float tireReturnSpeed = 80;
  float tireMaxRotation = 30;
};

struct Lobby {
  std::unordered_map<int, std::shared_ptr<Player>> players;
  std::unordered_map<int, Car> cars;

  void update(float dt) {
    for (auto &[id, car] : cars) {
      if (car.throttle != 0) {
        car.velocity += car.throttle * car.acceleration * dt;
      } else {
        if (car.velocity > 0) {
          car.velocity -= car.friction * dt;
          if (car.velocity < 0)
            car.velocity = 0;
        } else if (car.velocity < 0) {
          car.velocity += car.friction * dt;
          if (car.velocity > 0)
            car.velocity = 0;
        }
      }

      car.velocity =
          std::clamp(car.velocity, -car.maxVelocity, car.maxVelocity);

      if (car.steering != 0) {
        car.tireRotation += car.steering * car.tireTurnSpeed * dt;
      } else {
        if (car.tireRotation > 0) {
          car.tireRotation -= car.tireReturnSpeed * dt;
          if (car.tireRotation < 0)
            car.tireRotation = 0;
        } else if (car.tireRotation < 0) {
          car.tireRotation += car.tireReturnSpeed * dt;
          if (car.tireRotation > 0)
            car.tireRotation = 0;
        }
      }

      car.tireRotation = std::clamp(car.tireRotation, -car.tireMaxRotation,
                                    car.tireMaxRotation);

      car.rotation += car.tireRotation * car.turnSpeed * dt *
                      (car.velocity / car.maxVelocity);

      float rotationRad = car.rotation * M_PI / 180;

      car.x += std::sin(rotationRad) * car.velocity * dt;
      car.y -= std::cos(rotationRad) * car.velocity * dt;
    }

    std::vector<ServerCar> serverCars;
    for (auto &[id, c] : cars) {
      serverCars.push_back({c.x, c.y, c.rotation, id});
    }

    Message msg = LobbyState{serverCars};
    broadcast(msg);
  }

  void broadcast(const Message &msg) {
    for (auto &[_, p] : players) {
      p->ws->send(encode_message(msg), uWS::OpCode::BINARY);
    }
  }
};
