
// ROS Hexapod Locomotion Node
// Copyright (c) 2014, Kevin M. Ochs
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the <organization> nor the
//     names of its contributors may be used to endorse or promote products
//     derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: Kevin M. Ochs

#include <control.h>

//==============================================================================
// Constructor
//==============================================================================

Control::Control( void )
{
    prev_hex_state_ = false;
    hex_state_ = false;
    root_.y = 0.0;
    root_.x = 0.0;
    root_.yaw = 0.0;
    body_.y = 0.0;
    body_.z = 0.0;
    body_.x = 0.0;
    body_.pitch = 0.0;
    body_.yaw = 0.0;
    body_.roll = 0.0;
    head_.yaw = 0.0;
    for ( int leg_index = 0; leg_index <= 5; leg_index++ )
    {
        feet_.foot[leg_index].x = 0.0;
        feet_.foot[leg_index].y = 0.0;
        feet_.foot[leg_index].z = 0.0;
        feet_.foot[leg_index].yaw = 0.0;
        legs_.leg[leg_index].coxa = 0.0;
        legs_.leg[leg_index].femur = 0.0;
        legs_.leg[leg_index].tibia = 0.0;
        legs_.leg[leg_index].tarsus = 0.0;
    }
    root_sub_ = nh_.subscribe<hexapod_msgs::RootJoint>( "root", 50, &Control::rootCallback, this);
    body_sub_ = nh_.subscribe<hexapod_msgs::BodyJoint>( "body", 50, &Control::bodyCallback, this);
    head_sub_ = nh_.subscribe<hexapod_msgs::HeadJoint>( "head", 50, &Control::headCallback, this);
    state_sub_ = nh_.subscribe<hexapod_msgs::State>( "state", 5, &Control::stateCallback, this);
}

//==============================================================================
// Getter and Setters
//==============================================================================

void Control::setHexActiveState( bool state )
{
    hex_state_ = state;
}

bool Control::getHexActiveState( void )
{
    return hex_state_;
}

void Control::setPrevHexActiveState( bool state )
{
    prev_hex_state_ = state;
}

bool Control::getPrevHexActiveState( void )
{
    return prev_hex_state_;
}

//==============================================================================
// Topics we subscribe to
//==============================================================================

// rostopic pub /root hexapod_msgs/RootJoint '{yaw: 2.0 }'
// rostopic pub /state hexapod_msgs/State true
// rostopic pub /head hexapod_msgs/HeadJoint '{yaw: 2.0 }'

void Control::rootCallback( const hexapod_msgs::RootJointConstPtr &root_msg )
{
    root_.y = root_.y + ( root_msg->y - root_.y ) / 20.0; // 40 mm max
    root_.x = root_.x + ( root_msg->x - root_.x ) / 20.0; // 40 mm max
    root_.yaw = root_.yaw + ( root_msg->yaw - root_.yaw ) / 4.0; // 8 degrees max
}

void Control::bodyCallback( const hexapod_msgs::BodyJointConstPtr &body_msg )
{
    body_.pitch = body_.pitch + ( body_msg->pitch - body_.pitch ) / 30.0; // 12 degrees max
    body_.roll = body_.roll + ( body_msg->roll - body_.roll ) / 30.0; // 12 degrees max
}

void Control::headCallback( const hexapod_msgs::HeadJointConstPtr &head_msg )
{
    head_.yaw = head_msg->yaw; // 25 degrees max
}

void Control::stateCallback( const hexapod_msgs::StateConstPtr &state_msg )
{
    if (state_msg->active == true )
    {
        if ( getHexActiveState() == false )
        {
            body_.y = 0.0;
            body_.z = 0.0;
            body_.x = 0.0;
            body_.pitch = 0.0;
            body_.yaw = 0.0;
            body_.roll = 0.0;
            root_.y = 0.0;
            root_.x = 0.0;
            root_.yaw = 0.0;
            setHexActiveState( true );
            ROS_INFO("Hexapod locomotion is now active.");
        }
    }

    if ( state_msg->active == false )
    {
        if ( getHexActiveState() == true )
        {
            body_.y = 0.0;
            body_.x = 0.0;
            body_.pitch = 0.0;
            body_.yaw = 0.0;
            body_.roll = 0.0;
            root_.y = 0.0;
            root_.x = 0.0;
            root_.yaw = 0.0;
            setHexActiveState( false );
            ROS_WARN("Hexapod locomotion shutting down servos.");
        }
    }
}

