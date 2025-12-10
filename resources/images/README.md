# Image Assets for GUI

This directory contains the image resources for the Smart Task Management System GUI.

## Required Assets

Place the following image files in this directory:

### 1. bg_guild_board.jpg
- **Purpose**: Main interface background (公会任务板背景)
- **Format**: JPG
- **Size**: 1080x1920 pixels
- **Description**: Fantasy RPG adventurer guild quest board with wooden texture and warm candlelight atmosphere

### 2. texture_parchment.png
- **Purpose**: Task card background texture (任务卡片羊皮纸纹理)
- **Format**: PNG
- **Size**: 512x256 pixels (seamless/tileable pattern)
- **Description**: Old paper texture with worn edges and yellowed appearance

### 3. icon_stamp_completed.png
- **Purpose**: Completion stamp (完成印章)
- **Format**: PNG with transparent background
- **Size**: 512x512 pixels
- **Description**: Red ink stamp with "QUEST CLEAR" text

## Asset Integration Status

The QML code has been updated to use these assets:

- **MainView.qml**: Displays `bg_guild_board.jpg` as the window background
- **QuestCard.qml**: Uses `texture_parchment.png` as a tiled background for task cards
- **QuestCard.qml**: Shows `icon_stamp_completed.png` when a task is completed (via hold-to-complete gesture)

## Notes

- All assets are referenced in `resources/qml.qrc` and will be embedded in the executable when built
- The background image uses `PreserveAspectCrop` to fill the window
- The parchment texture uses `Tile` mode for seamless repetition
- The completion stamp appears with a -15° rotation and 0.9 opacity for a realistic effect
