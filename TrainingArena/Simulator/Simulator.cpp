#pragma once
#pragma GCC optimize("Ofast")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops")

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <chrono>
#include <vector>
#include <string>
#include <stdlib.h>

using namespace std;
using namespace std::chrono;

#define TIMEOUT 40000 //10 ms timeout
#define TIMEOUT2 900000
#define Now() chrono::high_resolution_clock::now()

static struct Stopwatch {
	chrono::high_resolution_clock::time_point c_time, c_timeout;
	void setTimeout(int us) { c_timeout = c_time + chrono::microseconds(us); }
	void Start() { c_time = Now(); }
	inline bool Timeout() { return Now() > c_timeout; }
	long long EllapsedMicroseconds() { return chrono::duration_cast<chrono::microseconds>(Now() - c_time).count(); }
	long long EllapsedMilliseconds() { return chrono::duration_cast<chrono::milliseconds>(Now() - c_time).count(); }
} stopwatch;

#include "..\..\Bots\Advanced\BasicAgent\BasicAgent.h"
#include "..\..\Bots\Advanced\MMGon\MinMax.h"

int main()
{
	int FIRST_PLAYER = 0;
	int MAX_TURNS = 50;

	Board simulator1(50);
	MinMaxGon::Board simulator2(50);

	simulator1.external_init();
	simulator2.external_init();


	BasicAgent::Agent agent1;
	MinMaxGon::Agent agent2;

	float KILL_HELIOS = 5.0;
	float MY_HELIOS_DANGER = -4.0;
	float ENEMY_HELIOS_DANGER = 0.05;
	float MY_THREAT = 2.5;
	float ENEMY_THREAT = -2.5;
	float KILL_PIECE = 2.5;
	agent1.set_coeffs({ KILL_HELIOS, MY_HELIOS_DANGER, ENEMY_HELIOS_DANGER, MY_THREAT, ENEMY_THREAT, KILL_PIECE });
	agent2.set_coeffs({ KILL_HELIOS, MY_HELIOS_DANGER, ENEMY_HELIOS_DANGER, MY_THREAT, ENEMY_THREAT, KILL_PIECE });
	//agent1.set_coeffs({ 1.5, -1.5, 14, -5.5, 5, 2, 14.5, -17.5, 10, 14.5, -12.5, -14.5 });

	stopwatch.Start();

	int games = 1000;

	int win1 = 0, win2 = 0;
	while (games > 0) {
		simulator2.external_start(games & 1);
		simulator1.external_start(games & 1);

		//cerr << simulator2 << endl;
		//cerr << simulator1 << endl;
		bool player_turn = games&1;

		while (!simulator1.external_ended()) {

			tuple<int,int,int,int,int> action;
			if (player_turn == 0)
				action = agent1.external_find_best_action(simulator1, player_turn, !player_turn);
			else
				action = agent2.external_find_best_action(simulator2, player_turn, !player_turn);
			
			simulator1.external_do_action(action);
			simulator2.external_do_action(action);

			//cerr << "----------------BOARD STATE 1---------------" << endl;
			//cerr << simulator1 << endl;
			//cerr << "----------------BOARD STATE 2---------------" << endl;
			//cerr << simulator2 << endl;
			player_turn = !player_turn;
		}

		if (simulator1.winner() == 0)
			cerr << "AGENT1 WINS, A1=" << ++win1 << " A2=" << win2 << endl;
		else if (simulator1.winner() == 1)
			cerr << "AGENT2 WINS, A1=" << win1 << " A2=" << ++win2 << endl;
		else
			cerr << "DRAW, A1=" << win1 << " A2=" << ++win2 << endl;
		games--;
	}
	cerr << "AGENT1 " << win1 << " AGENT2 " << win2 << endl;
	cerr << "TIME " << stopwatch.EllapsedMilliseconds() << endl;
	//cerr << simulator1 << endl;
}