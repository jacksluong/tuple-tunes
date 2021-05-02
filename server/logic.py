import sys
sys.path.append('__HOME__/server')
from in_game_functions import *


def request_handler(request):
    if request["method"] == "GET":
        #get the game id, username, and last updated measure
        try:
            username = request['values']['username']
            game_id = int(request['values']['game_id'])
            last_measure = int(request['values']['measure'])
        except:
            # return "Please provide username, game id, and last updated measure"
            return "-1"

        return fetch(game_id, username, last_measure)

    elif request["method"] == "POST":
        try:
            type_ = request['form']['type']
        except:
            # return "Please either post type new_measure or ping to update player last ping"
            return "-1"
        
        #new_measure - adds a measure to the song
        #ping - updates the ping

        #if we upload a new measure, we will update this info in our database
        if type_ == 'new_measure':
            try:
                username = request['form']['username']
                game_id = int(request['form']['game_id'])

                #parse measure
                measure_str = request['form']['measure'].split()
                measure = [int(str_val) for str_val in measure_str]

            except Exception as e:
                # return "Please post valid username, measure information, and game id with new turn"
                return "-1&{e}"
            
            #return measure
            return play_turn(game_id, username, measure)
        
        #if the post is a ping, we will update last ping of player
        elif type_ == 'ping':
            try:
                username = request['form']['username']
                game_id = int(request['form']['game_id'])
            except:
                # return "Please post valid username and game id with ping"
                return "-1"
            
            return update_last_ping(game_id, username)
        
        #request to end game
        elif type_ == "end":
            try:
                game_id = int(request['form']['game_id'])
            except:
                return "Please post valid username and game id"
            return end_game(game_id)

        else:
            return "Post either new measure or ping to logic handler"
