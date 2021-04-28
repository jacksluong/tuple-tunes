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
            return "Please provide username, game id, and last updated measure"

        return fetch(game_id, username, last_measure)

    elif request["method"] == "POST":
        pass