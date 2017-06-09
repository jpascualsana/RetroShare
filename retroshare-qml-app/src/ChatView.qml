/*
 * RetroShare Android QML App
 * Copyright (C) 2016-2017  Gioacchino Mazzurco <gio@eigenlab.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import org.retroshare.qml_components.LibresapiLocalClient 1.0
import "." //Needed for TokensManager singleton
import "./components"
Item
{
	id: chatView
	property string chatId
	property int token: 0

	property string objectName:"chatView"


	function refreshData()
	{
		console.log("chatView.refreshData()", visible)
		if(!visible) return

		rsApi.request( "/chat/messages/"+chatId, "", function(par)
		{
			chatModel.json = par.response
			token = JSON.parse(par.response).statetoken
			TokensManager.registerToken(token, refreshData)

			ChatCache.lastMessageCache.updateLastMessageCache(chatId, chatModel.json)

			if(chatListView.visible)
			{
				chatListView.positionViewAtEnd()
				rsApi.request("/chat/mark_chat_as_read/"+chatId)
			}
		} )
	}

	Component.onCompleted: refreshData()
	onFocusChanged: focus && refreshData()

	JSONListModel
	{
		id: chatModel
		query: "$.data[*]"
	}

	ListView
	{
		property var styles: StyleChat.chat
		id: chatListView
		width: parent.width - styles.bubbleMargin
		height: parent.height - inferiorPanel.height
		anchors.horizontalCenter: parent.horizontalCenter
		model: chatModel.model
		delegate: ChatBubbleDelegate {}
		spacing: styles.bubbleSpacing
		preferredHighlightBegin: 1

		onHeightChanged: {
			chatListView.currentIndex = count - 1
		}

	}

	Item {

		property var styles: StyleChat.inferiorPanel

		id: inferiorPanel
		height:  ( msgComposer.height > styles.height)? msgComposer.height: styles.height
		width: parent.width
		anchors.bottom: parent.bottom

		Rectangle {
			id: backgroundRectangle
			anchors.fill: parent.fill
			width: parent.width
			height: parent.height
			color:inferiorPanel.styles.backgroundColor
			border.color: inferiorPanel.styles.borderColor
		}

		BtnIcon {

			id: attachButton

			property var styles: StyleChat.inferiorPanel.btnIcon

			height: styles.height
			width: styles.width

			anchors.left: parent.left
			anchors.bottom: parent.bottom

			anchors.margins: styles.margin

			imgUrl: styles.attachIconUrl
		}


		TextArea
		{
			property var styles: StyleChat.inferiorPanel.msgComposer
			id: msgComposer

			anchors.verticalCenter: parent.verticalCenter
			anchors.left: attachButton.right

			height: setTextAreaHeight()

////
////				(contentHeight > font.pixelSize)? contentHeight +font.pixelSize : parent.styles.height


			width: chatView.width -
				   (sendButton.width + sendButton.anchors.margins) -
				   (attachButton.width + attachButton.anchors.margins) -
				   (emojiButton.width + emojiButton.anchors.margins)


			placeholderText: styles.placeHolder
			background: styles.background

			wrapMode: TextEdit.Wrap

			onTextChanged: {
				if (msgComposer.length == 0)
				{
					sendButton.state = ""
				}
				else if (msgComposer.length > 0)
				{
					sendButton.state = "SENDBTN"
				}
			}

			function setTextAreaHeight (){
				if (msgComposer.height >= chatView.height / msgComposer.styles.maxHeight)
				{
					return msgComposer.height
				}
				else if (contentHeight > font.pixelSize)
				{
					return msgComposer.contentHeight + msgComposer.font.pixelSize
				}
				else
				{
					return  parent.styles.height
				}
			}

		}

		BtnIcon {

			id: emojiButton

			property var styles: StyleChat.inferiorPanel.btnIcon

			height: styles.height
			width: styles.width

			anchors.right: sendButton.left
			anchors.bottom: parent.bottom

			anchors.margins: styles.margin

			imgUrl: styles.emojiIconUrl
		}

		BtnIcon {

			id: sendButton

			property var styles: StyleChat.inferiorPanel.btnIcon
			property alias icon: sendButton.imgUrl

			height: styles.height
			width: styles.width

			anchors.right: parent.right
			anchors.bottom: parent.bottom

			anchors.margins: styles.margin

			imgUrl: styles.microIconUrl

			onClicked:
			{
				if (sendButton.state == "SENDBTN" ) {
					var jsonData = {"chat_id":chatView.chatId, "msg":msgComposer.text}
					rsApi.request( "/chat/send_message", JSON.stringify(jsonData),
								   function(par) { msgComposer.text = ""; } )
				}
			}

			onPressed:
			{
				if (sendButton.state == "RECORDING" )
				{
					sendButton.state = ""
				}
				else if (sendButton.state == "" )
				{
					sendButton.state = "RECORDING"
				}
			}

			onReleased:
			{
				if (sendButton.state == "RECORDING" )
				{
					sendButton.state = ""
				}

			}


			states: [
				State {
					name: ""
					PropertyChanges { target: sendButton; icon: styles.microIconUrl}
				},
				State {
					name: "RECORDING"
					PropertyChanges { target: sendButton; icon: styles.microMuteIconUrl}
				},
				State {
					name: "SENDBTN"
					PropertyChanges { target: sendButton; icon: styles.sendIconUrl}
				}
			]
		}

	}
}
