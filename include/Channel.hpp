/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hutzig <hutzig@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 14:41:40 by hutzig            #+#    #+#             */
/*   Updated: 2025/04/30 14:41:41 by hutzig           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <unordered_set>

#include "Client.hpp"

#define MAX_USER 10

class Channel{
	public:
		Channel() = delete;
        Channel(std::string& name);
        Channel(const Channel&) = delete;
		Channel& operator=(const Channel& other) = delete;
		~Channel();

        std::string getName();
        std::string getPassword();
        std::string getTopic();
        bool        getInviteMode();
        bool        getTopicMode();
        bool        getPasswdMode();
        bool        getLimitMode();
        size_t      channelSize();

        // Channel's mode:
        void        setInviteOnly();
        void        unsetInviteOnly();
        
        void        setTopicRestrictions();
        void        unsetTopicRestrictions();

        void        setPassword();
        void        unsetPassword();

        void        setOperator();
        void        unsetOperator();

        void        setLimit();
        void        unsetLimit();


    private:
        std::string channel_name_;
        std::string channel_passwd_;
        std::string channel_topic_;
        bool        channel_invite_only_;
        bool        channel_restric_topic_;
        bool        channel_with_passwd_;
        bool        channel_user_limit_;
        size_t      user_limit_;

        std::unordered_set<Client*> users_; // Channels don’t own users; they just refer to them.
        std::unordered_set<Client*> operators_;
        std::unordered_set<Client*> invited_users_;

};
