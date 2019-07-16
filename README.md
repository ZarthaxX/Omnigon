# Omnigon

This is a project for the board game "Omnigon".There are mainly 2 parts:
- The interface folder: 
	Contains a program that provides functionality to play in different formats, human vs human, bot vs human or bot vs bot
- The bots folder: 
	Contains a set of bots that can be used to play against in the interface program, in addition to a StarterKit that is intended to guide you to start designing your own bot!

Interface
=========

This program is used to play Omnigon.It provides a set of commands to configure the match itself, listed below:

Usage
-----
```
--gold_player=<argument>
```
Takes the path to the bot program that will be used for gold_player.
The default value for gold_player will be set to human player.
```
--white_player=<argument>
```
Takes the path to the bot program that will be used for white_player.
The default value for white_player will be set to human player.
```
--games=<argument>
```
Takes the amount of games that will be played between the 2 players.
The default value is set to 1.
```
--max_turns=<argument>
```
Takes the max amount of turns that each game will last.
The default value is set to 50.

```
--first=<argument>
```
Takes the player that will play first on the first game,the possible values of argument are : "gold", "white".
The default value is set to "gold".
```
--show=<argument>
```
Takes a boolean value that indicates if the games will be showed in the UI.It will do so if the value is set to true, and will not if it is false.
The default value is set to true.

Bots
====
Contains implementations of different bots, each with a higher level of play.
It also contains a starterKit that is used to guide someone that wishes to implement a bot for Omnigon.
This starterKit contains the structure that one would need to implement to read the input from the interface program detailed above as well as the output format.