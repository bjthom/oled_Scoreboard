import urllib2
import time
import xml.etree.ElementTree as ET
import serial

team_list = ['ana','ari','atl','bal','bos','cha',
             'chn','cin','cle','col','det','flo', 
             'hou','kca','lan','mil','min','nya', 
             'nyn','oak','phi','pit','sdn','sea', 
             'sfn','sln','tba','tex','tor','was']

def scoreboard(gid):
    temp_id = gid.split('_')
    year = 'year_' + temp_id[1] + '/'
    month = 'month_' + temp_id[2] + '/'
    day = 'day_' + temp_id[3] + '/'
    url_setup = 'http://gd2.mlb.com/components/game/mlb/' + year + month + day + gid
    url = url_setup + '/plays.xml'
    url_game = url_setup + '/game.xml'
    url_event = url_setup + '/game_events.xml'

    game_info = urllib2.urlopen(url_game)
    game_tree = ET.parse(game_info)
    info = game_tree.getroot()
    game_time = info.attrib['game_time_et']
    
    away_team = temp_id[4][:3]
    home_team = temp_id[5][:3]

    teams = {'ana':' 0', 'ari':' 1', 'atl':' 2', 'bal':' 3', 'bos':' 4', 'cha':' 5', \
             'chn':' 6', 'cin':' 7', 'cle':' 8', 'col':' 9', 'det':'10', 'flo':'11', \
             'hou':'12', 'kca':'13', 'lan':'14', 'mil':'15', 'min':'16', 'nya':'17', \
             'nyn':'18', 'oak':'19', 'phi':'20', 'pit':'21', 'sdn':'22', 'sea':'23', \
             'sfn':'24', 'sln':'25', 'tba':'26', 'tex':'27', 'tor':'28', 'was':'29'}

    at = teams[away_team]
    ht = teams[home_team]
    inn = 't'
##    print at
##    print ht
    
    ##ser = serial.Serial(6, 9600)
    while 1:
        try:
            data = urllib2.urlopen(url)
            tree = ET.parse(data)
            
            game = tree.getroot()
            game_kids = game.getchildren()
            score = game_kids[0].attrib

            balls = game.attrib['b']
            strikes = game.attrib['s']
            outs = game.attrib['o']

            hh = score['hh']
            ar = score['ar']
            hr = score['hr']
            ah = score['ah']
            ae = score['ae']
            he = score['he']

            if len(hh) == 1:
                hh = ' ' + hh
            if len(ar) == 1:
                ar = ' ' + ar
            if len(hr) == 1:
                hr = ' ' + hr
            if len(ah) == 1:
                ah = ' ' + ah
            
            inning = game.attrib['inning']
            topbot = game.attrib['inning_state']
            status = game.attrib['status_ind']

            if len(inning) == 1:
                inning = ' ' + inning

            if status == 'I':
                if topbot == 'Top':
                    inn = 't'
                if topbot == 'Middle':
                    inn = 'm'
                if topbot == 'Bottom':
                    inn = 'b'
                if topbot == 'End':
                    inn = 'e'
            elif status == 'P':
                inn = 'p'
            elif status == 'PW':
                inn = 'w'
            elif status == 'F' or 'O':
                inn = 'f'
            else:
                inn = '0'

            offense = game_kids[4].getchildren()[0]

            if (len(offense)-1) == 3:
                on_base = 3
            elif (len(offense)-1) == 2:
                on_base = 2
            elif (len(offense)-1) == 1:
                on_base = 1
            else:
                on_base = 0
            
            at_bat = game_kids[3].getchildren()
##            print len(at_bat)
            if len(at_bat) == 0:
                last_play = 'None'
            else:
                last_play = at_bat[-1].attrib['des']

            if len(last_play) < 20:
                addon = 20 - len(last_play)
                spaces = "                    "
                last_play = last_play + spaces[0:addon]
            if len(last_play) > 20:
                last_play = last_play[:20]
            
            rhe = ar+ah+ae+hr+hh+he
            count = balls + strikes + outs
            gameinfo = at+ht+rhe+count+inning+inn
            ser_data = gameinfo+last_play
            print 'Count: ' + count
            print 'RHE: ' + rhe
            print 'Inn: ' + inning + inn
            print on_base
            print 'Last Play: ' + last_play
            print len(last_play)
            print ser_data
            print '==='

            ##ser.write(ser_data)
                
            time.sleep(15)
        except KeyboardInterrupt:
            ser.close()
            print 'Scoreboard Done.'
            break

