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

#define USER_MAX 100

class Channel{
	public:
		Channel() = delete;
        Channel(std::string& name, Client& user);
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

        // Channel's set mode:
        void        setInviteOnly();
        void        unsetInviteOnly();
        
        void        setTopicRestrictions();
        void        unsetTopicRestrictions();

        void        setPassword();
        void        unsetPassword();
        void        addNewPassword(const std::string& key);

        void        setLimit();
        void        unsetLimit();
        void        addLimit(int limit);

        // Regular user and Channel Operator:
        void        addNewUser(Client& user);
        void        removeUser(Client& user);

        void        addNewOperator(Client& user);
        void        removeOperator(Client& user);
        bool        isChannelOperator(Client& user);

        // Channel Operators commands:
        // void        kickUser(Client& user, Client& target, const std::string& reason);
        // void        inviteUser(Client& user, Client& target);
        // newTopic()
        // changeMode()
        // handleInviteOnlyMode()
        // handleTopicRestrictionMode()
        // handlePasswordMode()

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
