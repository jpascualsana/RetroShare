/*******************************************************************************
 * libretroshare/src/retroshare: rsgxschannels.h                               *
 *                                                                             *
 * libretroshare: retroshare core library                                      *
 *                                                                             *
 * Copyright (C) 2012  Robert Fernie <retroshare@lunamutt.com>                 *
 * Copyright (C) 2018-2019  Gioacchino Mazzurco <gio@eigenlab.org>             *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Lesser General Public License as              *
 * published by the Free Software Foundation, either version 3 of the          *
 * License, or (at your option) any later version.                             *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public License    *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.       *
 *                                                                             *
 *******************************************************************************/
#pragma once

#include <cstdint>
#include <string>
#include <list>
#include <functional>

#include "retroshare/rstokenservice.h"
#include "retroshare/rsgxsifacehelper.h"
#include "retroshare/rsgxscommon.h"
#include "serialiser/rsserializable.h"
#include "retroshare/rsturtle.h"
#include "util/rsdeprecate.h"
#include "retroshare/rsgxscircles.h"
#include "util/rsmemory.h"

class RsGxsChannels;

/**
 * Pointer to global instance of RsGxsChannels service implementation
 * @jsonapi{development}
 */
extern RsGxsChannels* rsGxsChannels;


struct RsGxsChannelGroup : RsSerializable
{
    RsGxsChannelGroup() : mAutoDownload(false) {}

	RsGroupMetaData mMeta;
	std::string mDescription;
	RsGxsImage mImage;

	bool mAutoDownload;

	/// @see RsSerializable
	virtual void serial_process( RsGenericSerializer::SerializeJob j,
	                             RsGenericSerializer::SerializeContext& ctx )
	{
		RS_SERIAL_PROCESS(mMeta);
		RS_SERIAL_PROCESS(mDescription);
		RS_SERIAL_PROCESS(mImage);
		RS_SERIAL_PROCESS(mAutoDownload);
	}
};

struct RsGxsChannelPost : RsSerializable
{
	RsGxsChannelPost() : mCount(0), mSize(0) {}

	RsMsgMetaData mMeta;

	std::set<RsGxsMessageId> mOlderVersions;
	std::string mMsg;  // UTF8 encoded.

	std::list<RsGxsFile> mFiles;
	uint32_t mCount;   // auto calced.
	uint64_t mSize;    // auto calced.

	RsGxsImage mThumbnail;

	/// @see RsSerializable
	virtual void serial_process( RsGenericSerializer::SerializeJob j,
	                             RsGenericSerializer::SerializeContext& ctx )
	{
		RS_SERIAL_PROCESS(mMeta);
		RS_SERIAL_PROCESS(mOlderVersions);

		RS_SERIAL_PROCESS(mMsg);
		RS_SERIAL_PROCESS(mFiles);
		RS_SERIAL_PROCESS(mCount);
		RS_SERIAL_PROCESS(mSize);
		RS_SERIAL_PROCESS(mThumbnail);
	}
};


class RsGxsChannels: public RsGxsIfaceHelper, public RsGxsCommentService
{
public:
	explicit RsGxsChannels(RsGxsIface& gxs) : RsGxsIfaceHelper(gxs) {}
	virtual ~RsGxsChannels() {}

	/**
	 * @brief Create channel. Blocking API.
	 * @jsonapi{development}
	 * @param[in]  name         Name of the channel
	 * @param[in]  description  Description of the channel
	 * @param[in]  thumbnail    Optional image to show as channel thumbnail.
	 * @param[in]  authorId     Optional id of the author. Leave empty for an
	 *                          anonymous channel.
	 * @param[in]  circleType   Optional visibility rule, default public.
	 * @param[in]  circleId     If the channel is not public specify the id of
	 *                          the circle who can see the channel. Depending on
	 *                          the value you pass for
	 *                          circleType this should be be an external circle
	 *                          if EXTERNAL is passed, a local friend group id
	 *                          if NODES_GROUP is passed, empty otherwise.
	 * @param[out] channelId    Optional storage for the id of the created
	 *                          channel, meaningful only if creations succeeds.
	 * @param[out] errorMessage Optional storage for error messsage, meaningful
	 *                          only if creation fail.
	 * @return False on error, true otherwise.
	 */
	virtual bool createChannelV2(
	        const std::string& name,
	        const std::string& description,
	        const RsGxsImage&  thumbnail = RsGxsImage(),
	        const RsGxsId&     authorId = RsGxsId(),
	        RsGxsCircleType    circleType = RsGxsCircleType::PUBLIC,
	        const RsGxsCircleId& circleId = RsGxsCircleId(),
	        RsGxsGroupId& channelId    = RS_DEFAULT_STORAGE_PARAM(RsGxsGroupId),
	        std::string&  errorMessage = RS_DEFAULT_STORAGE_PARAM(std::string)
	        ) = 0;

	/**
	 * @brief Add a comment on a post or on another comment. Blocking API.
	 * @jsonapi{development}
	 * @param[in]  channelId Id of the channel in which the comment is to be
	 *                       posted
	 * @param[in]  threadId  Id of the post (that is a thread) in the channel
	 *                       where the comment is placed
	 * @param[in]  parentId  Id of the parent of the comment that is either a
	 *                       channel post Id or the Id of another comment.
	 * @param[in]  authorId  Id of the author of the comment
	 * @param[in]  comment   UTF-8 string containing the comment itself
	 * @param[out] commentMessageId Optional storage for the id of the comment
	 *                              that was created, meaningful only on success.
	 * @param[out] errorMessage Optional storage for error message, meaningful
	 *                          only on failure.
	 * @return false on error, true otherwise
	 */
	virtual bool createCommentV2(
	        const RsGxsGroupId&   channelId,
	        const RsGxsMessageId& threadId,
	        const RsGxsMessageId& parentId,
	        const RsGxsId&        authorId,
	        const std::string&    comment,
	        RsGxsMessageId&       commentMessageId = RS_DEFAULT_STORAGE_PARAM(RsGxsMessageId),
	        std::string&          errorMessage     = RS_DEFAULT_STORAGE_PARAM(std::string)
	        ) = 0;

	/**
	 * @brief Create channel post. Blocking API.
	 * @jsonapi{development}
	 * @param[in]  channelId    Id of the channel where to put the post. Beware
	 *                          you need publish rights on that channel to post.
	 * @param[in]  title        Title of the post
	 * @param[in]  mBody        Text content of the post
	 * @param[in]  files        Optional list of attached files. These are
	 *                          supposed to be already shared,
	 *                          @see ExtraFileHash() below otherwise.
	 * @param[in]  thumbnail    Optional thumbnail image for the post.
	 * @param[in]  origPostId   If this is supposed to replace an already
	 *                          existent post, the id of the old post. If left
	 *                          blank a new post will be created.
	 * @param[out] postId       Optional storage for the id of the created post,
	 *                          meaningful only on success.
	 * @param[out] errorMessage Optional storage for the error message,
	 *                          meaningful only on failure.
	 * @return false on error, true otherwise
	 */
	virtual bool createPostV2(
	        const RsGxsGroupId& channelId, const std::string& title,
	        const std::string& mBody,
	        const std::list<RsGxsFile>& files = std::list<RsGxsFile>(),
	        const RsGxsImage& thumbnail = RsGxsImage(),
	        const RsGxsMessageId& origPostId = RsGxsMessageId(),
	        RsGxsMessageId& postId = RS_DEFAULT_STORAGE_PARAM(RsGxsMessageId),
	        std::string& errorMessage = RS_DEFAULT_STORAGE_PARAM(std::string) ) = 0;

	/**
	 * @brief Create a vote
	 * @jsonapi{development}
	 * @param[in]  channelId    Id of the channel where to vote
	 * @param[in]  postId       Id of the channel post of which a comment is
	 *                          voted.
	 * @param[in]  commentId    Id of the comment that is voted
	 * @param[in]  authorId     Id of the author. Needs to be of an owned
	 *                          identity.
	 * @param[in]  vote         Vote value, either RsGxsVoteType::DOWN or
	 *                          RsGxsVoteType::UP
	 * @param[out] voteId       Optional storage for the id of the created vote,
	 *                          meaningful only on success.
	 * @param[out] errorMessage Optional storage for error message, meaningful
	 *                          only on failure.
	 * @return false on error, true otherwise
	 */
	virtual bool createVoteV2(
	        const RsGxsGroupId& channelId, const RsGxsMessageId& postId,
	        const RsGxsMessageId& commentId, const RsGxsId& authorId,
	        RsGxsVoteType vote,
	        RsGxsMessageId& voteId = RS_DEFAULT_STORAGE_PARAM(RsGxsMessageId),
	        std::string& errorMessage = RS_DEFAULT_STORAGE_PARAM(std::string) ) = 0;

	/**
	 * @brief Edit channel details.
	 * @jsonapi{development}
	 * @param[in] channel Channel data (name, description...) with modifications
	 * @return false on error, true otherwise
	 */
	virtual bool editChannel(RsGxsChannelGroup& channel) = 0;

	/**
	 * @brief Share extra file
	 * Can be used to share extra file attached to a channel post
	 * @jsonapi{development}
	 * @param[in] path file path
	 * @return false on error, true otherwise
	 */
	virtual bool ExtraFileHash(const std::string& path) = 0;

	/**
	 * @brief Remove extra file from shared files
	 * @jsonapi{development}
	 * @param[in] hash hash of the file to remove
	 * @return false on error, true otherwise
	 */
	virtual bool ExtraFileRemove(const RsFileHash& hash) = 0;

	/**
	 * @brief Get auto-download option value for given channel
	 * @jsonapi{development}
	 * @param[in] channelId channel id
	 * @param[out] enabled storage for the auto-download option value
	 * @return false if something failed, true otherwhise
	 */
	virtual bool getChannelAutoDownload(
	        const RsGxsGroupId& channelId, bool& enabled ) = 0;

	/**
	 * @brief Get download directory for the given channel
	 * @jsonapi{development}
	 * @param[in] channelId id of the channel
	 * @param[out] directory reference to string where to store the path
	 * @return false on error, true otherwise
	 */
	virtual bool getChannelDownloadDirectory( const RsGxsGroupId& channelId,
	                                          std::string& directory ) = 0;

	/**
	 * @brief Get channels summaries list. Blocking API.
	 * @jsonapi{development}
	 * @param[out] channels list where to store the channels
	 * @return false if something failed, true otherwhise
	 */
	virtual bool getChannelsSummaries(std::list<RsGroupMetaData>& channels) = 0;

	/**
	 * @brief Get channels information (description, thumbnail...).
	 * Blocking API.
	 * @jsonapi{development}
	 * @param[in] chanIds ids of the channels of which to get the informations
	 * @param[out] channelsInfo storage for the channels informations
	 * @return false if something failed, true otherwhise
	 */
	virtual bool getChannelsInfo(
	        const std::list<RsGxsGroupId>& chanIds,
	        std::vector<RsGxsChannelGroup>& channelsInfo ) = 0;

	/**
	 * @brief Get channel contents
	 * @jsonapi{development}
	 * @param[in] channelId id of the channel of which the content is requested
	 * @param[in] contentsIds ids of requested contents
	 * @param[out] posts storage for posts
	 * @param[out] comments storage for the comments
	 * @return false if something failed, true otherwhise
	 */
	virtual bool getChannelContent( const RsGxsGroupId& channelId,
	                       const std::set<RsGxsMessageId>& contentsIds,
	                       std::vector<RsGxsChannelPost>& posts,
	                       std::vector<RsGxsComment>& comments ) = 0;

	/**
	 * @brief Get channel content summaries
	 * @jsonapi{development}
	 * @param[in] channelId id of the channel of which the content is requested
	 * @param[out] summaries storage for summaries
	 * @return false if something failed, true otherwhise
	 */
	virtual bool getContentSummaries( const RsGxsGroupId& channelId,
	                                  std::vector<RsMsgMetaData>& summaries ) = 0;

	/**
	 * @brief Toggle post read status. Blocking API.
	 * @jsonapi{development}
	 * @param[in] postId post identifier
	 * @param[in] read true to mark as read, false to mark as unread
	 * @return false on error, true otherwise
	 */
	virtual bool markRead(const RsGxsGrpMsgIdPair& postId, bool read) = 0;

	/**
	 * @brief Enable or disable auto-download for given channel. Blocking API
	 * @jsonapi{development}
	 * @param[in] channelId channel id
	 * @param[in] enable true to enable, false to disable
	 * @return false if something failed, true otherwhise
	 */
	virtual bool setChannelAutoDownload(
	        const RsGxsGroupId& channelId, bool enable ) = 0;

	/**
	 * @brief Share channel publishing key
	 * This can be used to authorize other peers to post on the channel
	 * @jsonapi{development}
	 * @param[in] channelId id of the channel
	 * @param[in] peers peers to share the key with
	 * @return false on error, true otherwise
	 */
	virtual bool shareChannelKeys(
	        const RsGxsGroupId& channelId, const std::set<RsPeerId>& peers ) = 0;

	/**
	 * @brief Set download directory for the given channel. Blocking API.
	 * @jsonapi{development}
	 * @param[in] channelId id of the channel
	 * @param[in] directory path
	 * @return false on error, true otherwise
	 */
	virtual bool setChannelDownloadDirectory(
	        const RsGxsGroupId& channelId, const std::string& directory) = 0;

	/**
	 * @brief Subscrbe to a channel. Blocking API
	 * @jsonapi{development}
	 * @param[in] channelId Channel id
	 * @param[in] subscribe true to subscribe, false to unsubscribe
	 * @return false on error, true otherwise
	 */
	virtual bool subscribeToChannel( const RsGxsGroupId& channelId,
	                                 bool subscribe ) = 0;

	/**
	 * @brief Request remote channels search
	 * @jsonapi{development}
	 * @param[in] matchString string to look for in the search
	 * @param multiCallback function that will be called each time a search
	 * result is received
	 * @param[in] maxWait maximum wait time in seconds for search results
	 * @return false on error, true otherwise
	 */
	virtual bool turtleSearchRequest(
	        const std::string& matchString,
	        const std::function<void (const RsGxsGroupSummary& result)>& multiCallback,
	        rstime_t maxWait = 300 ) = 0;

	/**
	 * @brief Request remote channel
	 * @jsonapi{development}
	 * @param[in] channelId id of the channel to request to distants peers
	 * @param multiCallback function that will be called each time a result is
	 *	received
	 * @param[in] maxWait maximum wait time in seconds for search results
	 * @return false on error, true otherwise
	 */
	virtual bool turtleChannelRequest(
	        const RsGxsGroupId& channelId,
	        const std::function<void (const RsGxsChannelGroup& result)>& multiCallback,
	        rstime_t maxWait = 300 ) = 0;

	/**
	 * @brief Search local channels
	 * @jsonapi{development}
	 * @param[in] matchString string to look for in the search
	 * @param multiCallback function that will be called for each result
	 * @param[in] maxWait maximum wait time in seconds for search results
	 * @return false on error, true otherwise
	 */
	virtual bool localSearchRequest(
	        const std::string& matchString,
	        const std::function<void (const RsGxsGroupSummary& result)>& multiCallback,
	        rstime_t maxWait = 30 ) = 0;


	/* Following functions are deprecated as they expose internal functioning
	 * semantic, instead of a safe to use API */

	/**
	 * @brief Create channel. Blocking API.
	 * @jsonapi{development}
	 * @deprecated { substituted by createChannelV2 }
	 * @param[inout] channel Channel data (name, description...)
	 * @return false on error, true otherwise
	 */
	RS_DEPRECATED_FOR(createChannelV2)
	virtual bool createChannel(RsGxsChannelGroup& channel) = 0;

	RS_DEPRECATED_FOR(getChannelsInfo)
	virtual bool getGroupData(const uint32_t &token, std::vector<RsGxsChannelGroup> &groups) = 0;

	RS_DEPRECATED_FOR(getChannelsContent)
	virtual bool getPostData(const uint32_t &token, std::vector<RsGxsChannelPost> &posts, std::vector<RsGxsComment> &cmts) = 0;

	RS_DEPRECATED_FOR(getChannelsContent)
	virtual bool getPostData(const uint32_t &token, std::vector<RsGxsChannelPost> &posts) = 0;

	/**
	 * @brief toggle message read status
	 * @deprecated
	 * @param[out] token GXS token queue token
	 * @param[in] msgId
	 * @param[in] read
	 */
	RS_DEPRECATED_FOR(markRead)
	virtual void setMessageReadStatus(
	        uint32_t& token, const RsGxsGrpMsgIdPair& msgId, bool read) = 0;

	/**
	 * @brief Share channel publishing key
	 * This can be used to authorize other peers to post on the channel
	 * @deprecated
	 * @param[in] groupId Channel id
	 * @param[in] peers peers to which share the key
	 * @return false on error, true otherwise
	 */
	RS_DEPRECATED_FOR(shareChannelKeys)
	virtual bool groupShareKeys(
	        const RsGxsGroupId& groupId, const std::set<RsPeerId>& peers ) = 0;

	/**
	 * @brief Request subscription to a group.
	 * The action is performed asyncronously, so it could fail in a subsequent
	 * phase even after returning true.
	 * @deprecated
	 * @param[out] token Storage for RsTokenService token to track request
	 * status.
	 * @param[in] groupId Channel id
	 * @param[in] subscribe
	 * @return false on error, true otherwise
	 */
	RS_DEPRECATED_FOR(subscribeToChannel)
	virtual bool subscribeToGroup( uint32_t& token, const RsGxsGroupId &groupId,
	                               bool subscribe ) = 0;

	/**
	 * @brief Request channel creation.
	 * The action is performed asyncronously, so it could fail in a subsequent
	 * phase even after returning true.
	 * @deprecated
	 * @param[out] token Storage for RsTokenService token to track request
	 * status.
	 * @param[in] group Channel data (name, description...)
	 * @return false on error, true otherwise
	 */
	RS_DEPRECATED_FOR(createChannelV2)
	virtual bool createGroup(uint32_t& token, RsGxsChannelGroup& group) = 0;

	/**
	 * @brief Add a comment on a post or on another comment
	 * @jsonapi{development}
	 * @deprecated
	 * @param[inout] comment
	 * @return false on error, true otherwise
	 */
	RS_DEPRECATED_FOR(createCommentV2)
	virtual bool createComment(RsGxsComment& comment) = 0;

	/**
	 * @brief Create channel post. Blocking API.
	 * @jsonapi{development}
	 * @deprecated
	 * @param[inout] post
	 * @return false on error, true otherwise
	 */
	RS_DEPRECATED_FOR(createPostV2)
	virtual bool createPost(RsGxsChannelPost& post) = 0;

	/**
	 * @brief Request post creation.
	 * The action is performed asyncronously, so it could fail in a subsequent
	 * phase even after returning true.
	 * @deprecated
	 * @param[out] token Storage for RsTokenService token to track request
	 * status.
	 * @param[in] post
	 * @return false on error, true otherwise
	 */
	RS_DEPRECATED_FOR(createPostV2)
	virtual bool createPost(uint32_t& token, RsGxsChannelPost& post) = 0;

	/**
	 * @brief createVote
	 * @jsonapi{development}
	 * @deprecated
	 * @param[inout] vote
	 * @return false on error, true otherwise
	 */
	RS_DEPRECATED_FOR(createVoteV2)
	virtual bool createVote(RsGxsVote& vote) = 0;

	/**
	 * @brief Request channel change.
	 * The action is performed asyncronously, so it could fail in a subsequent
	 * phase even after returning true.
	 * @deprecated
	 * @param[out] token Storage for RsTokenService token to track request
	 * status.
	 * @param[in] group Channel data (name, description...) with modifications
	 * @return false on error, true otherwise
	 */
	RS_DEPRECATED_FOR(editChannel)
	virtual bool updateGroup(uint32_t& token, RsGxsChannelGroup& group) = 0;

	//////////////////////////////////////////////////////////////////////////////
    ///                     Distant synchronisation methods                    ///
    //////////////////////////////////////////////////////////////////////////////
    ///
	RS_DEPRECATED_FOR(turtleChannelRequest)
	virtual TurtleRequestId turtleGroupRequest(const RsGxsGroupId& group_id)=0;
	RS_DEPRECATED
	virtual TurtleRequestId turtleSearchRequest(const std::string& match_string)=0;
	RS_DEPRECATED_FOR(turtleSearchRequest)
	virtual bool retrieveDistantSearchResults(TurtleRequestId req, std::map<RsGxsGroupId, RsGxsGroupSummary> &results) =0;
	RS_DEPRECATED
	virtual bool clearDistantSearchResults(TurtleRequestId req)=0;
	RS_DEPRECATED_FOR(turtleChannelRequest)
	virtual bool retrieveDistantGroup(const RsGxsGroupId& group_id,RsGxsChannelGroup& distant_group)=0;
	//////////////////////////////////////////////////////////////////////////////
};
