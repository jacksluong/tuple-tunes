import sys
sys.path.append('__HOME__/server')
from start_functions import *

def request_handler(request):

    #POST REQUEST HANDLERS
    if request["method"] == "POST":
        try:
            type_ = request['form']['type']
            #types of post requests:
                #create - creates a new game
                #join - join a game
                #start - start a game

        except Exception as e:
            # return "Error: please return a type - create, join, start, measure, ping"
            return "-1"

        # host creates game
        if type_ == "create":
            # return request
            try:
                host = request['form']['username']
                key = int(request['form']['key'])
                tempo = int(request['form']['tempo'])
            except Exception as e:
                # return "Please return host username, key, tempo whens creating a game"
                return "-1"

            return create_game(host, key, tempo)


        #players join game
        elif type_ == "join":

            #get values from request
            try:
                username = request['form']['username']
                game_code = int(request['form']['game_code'])
            except Exception as e:

                #not enough data provided
                # return "Please provide username and game code when joining a game"
                return "-1"
            
            return join_game(username, game_code)

        #start a game
        elif type_ == "start":
            
            #get values from request
            try:
                game_id = int(request['form']['game_id'])
            except:
                return "Please provide a valid game id"
            
            garbage_collect_games()
            return start_game(game_id)
        else:
            # return "Invalid POST request! Please specify valid type"
            return "-1"

    #GET REQUEST HANDLERS
    elif request["method"] == "GET":
        try:
            game_id = int(request['values']['game_id'])
        except:
            # return "Please provide valid game id!"
            return "-1"


        return fetch_game_status(game_id)

    #INVALID REQUEST
    else:
        # return 'INVALID REQUEST TYPE: ONLY POST OR GET REQUESTS'
        return "-1"