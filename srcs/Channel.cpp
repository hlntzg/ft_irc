/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hutzig <hutzig@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 14:42:06 by hutzig            #+#    #+#             */
/*   Updated: 2025/04/30 14:42:07 by hutzig           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(){
}

Channel::Channel(std::string& name) : channel_name_(name) {
    channel_passwd_ = "";
    channel_topic_ = "";
    channel_invite_only_ = false;
    channel_with_passwd_ = false;
    channel_restric_topic_ = false;
    channel_user_limit_ = false;
    user_limit_ = MAX_USER;
}

Channel::~Channel(){
}

std::string Channel::getName(){
    return channel_name_;
}

std::string Channel::getPassword(){
    return channel_passwd_;
}

std::string Channel::getTopic(){
    return channel_topic_;
}

bool    Channel::getInviteMode(){
    return channel_invite_only_;
}

bool    Channel::getTopicMode(){
    return channel_restric_topic_;
}

bool    Channel::getPasswdMode(){
    return channel_with_passwd_;
}

bool    Channel::getLimitMode(){
    return channel_user_limit_;
}

size_t  Channel::channelSize(){
    return users_.size();
}

// Channel's mode:
