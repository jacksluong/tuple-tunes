import numpy
import sqlite3
import datetime

moosic_db = '/var/jail/home/team59/moosic/moosic.db'


def fetch(game_id, username, last_updated_measure):
    '''given the game id, username, and last updated measure of fetching player,
    checks if game state needs to be updated,
    if so, returns the new game state along with the player who's turn it is'''

    with sqlite3.connect(moosic_db) as c:
        #update the player last ping for timeout purposes
        c.execute('''UPDATE players SET last_ping = ? WHERE game_id = ? AND username = ?;''', (datetime.datetime.now(), game_id, username))

        #check for status of the game
        game_status = c.execute('''SELECT game_status FROM games WHERE rowid = ?;''', (game_id, )).fetchone()

        if game_status[0] == 'ended':
            return f"Your game with game_id {game_id} has ended"
        
        #if game is started status, fetch returns all current players
        if game_status[0] == 'start':
            players = c.execute('''SELECT username FROM players WHERE game_id = ?;''',(game_id,)).fetchall()
            num_players = len(players)

            #get player names
            player_names = [val[0] for val in players]

            return f"Game has not started \n Num Players: {num_players} \n Current Players: {', '.join(player_names)}"

        #if currently in game: 
        if game_status[0] == 'in_game':

            #check if we need to update the number of measures
            current_measure, turn = c.execute('''SELECT measure, turn FROM games WHERE game_id = ?;''', (game_id, )).fetchone()

            if last_updated_measure == current_measure:
                return f"Your info is all up to date :) currently game is at measure {last_updated_measure}"
            
            #if we need to update things, we return the whole entire song again
            song_info = c.execute('''SELECT * FROM measures WHERE game_id = ? ORDER BY measure_number ASC;''', (game_id,)).fetchall()
            song = ''
            for measure in song_info:
                song = song + ' '.join(measure[3:18]) + '\n'
            
            #c.execute('''SELECT n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13,n14,n15,16)

            #figure out who's turn it should be
            players = c.execute('''SELECT username FROM players WHERE game_id = ? ORDER BY entry_time ASC;''',(game_id,)).fetchall()
            in_turn = players[turn][0] #username of player in turn

            return f"Player in turn: {in_turn} \n Current Song: \n {song}"

        
        return f"No matching game status found for {game_status[0]}"



