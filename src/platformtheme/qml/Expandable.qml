// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.7

Item {
	default property Item contentItem: null

	id: root
	property bool childVisible: false
	implicitHeight: childVisible ? contentItem.height : 0
	implicitWidth: contentItem.implicitWidth
	clip: true

	enum Direction {
		FromBottom,
		FromTop
	}
	property int direction: Expandable.Direction.FromBottom

	Behavior on implicitHeight {
		NumberAnimation {
			duration: 250
			easing.type: Easing.InOutQuart
		}
	}

	states: [
		State {
			name: "fromTop"
			when: root.direction === Expandable.Direction.FromTop

			AnchorChanges {
				target: contentItem

				anchors.bottom: root.bottom
				anchors.top: undefined
			}
		},
		State {
			name: "fromBottom"
			when: root.direction === Expandable.Direction.FromBottom

			AnchorChanges {
				target: contentItem

				anchors.bottom: undefined
				anchors.top: root.top
			}
		}
	]

	onContentItemChanged: contentItem.parent = this
}
