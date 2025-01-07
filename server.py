import sqlite3
import sys  # used to get argv
import cgi  # used to parse Multipart FormData
import time  # used for timestamps
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs

# Constants
GAME_TIME = 300

# Initialize the database
def initialize_db():
    connection = sqlite3.connect('hostage_chess.db')
    cursor = connection.cursor()

    # Create the games table
    cursor.execute('''CREATE TABLE IF NOT EXISTS games (
        GAME_NO INTEGER PRIMARY KEY AUTOINCREMENT,
        WHITE_HANDLE TEXT NOT NULL,
        BLACK_HANDLE TEXT,
        RESULT TEXT CHECK(length(RESULT) <= 256)
    )''')

    # Create the game_states table
    cursor.execute('''CREATE TABLE IF NOT EXISTS game_states (
        GAME_NO INTEGER,
        TURN_NO INTEGER,
        TURN TEXT CHECK(TURN IN ('w', 'b')),
        BOARD TEXT NOT NULL,
        REAL_TIME TEXT NOT NULL,
        WHITE_TIME INTEGER,
        BLACK_TIME INTEGER,
        PRIMARY KEY (GAME_NO, TURN_NO),
        FOREIGN KEY (GAME_NO) REFERENCES games(GAME_NO)
    )''')

    connection.commit()

    connection.close()

# Web server handler
class MyHandler(BaseHTTPRequestHandler):
    
    # Serve the GET requests
    def do_GET(self):
        parsed = urlparse(self.path)
   
        if parsed.path == '/history.html':
            connection = sqlite3.connect('hostage_chess.db')
            cursor = connection.cursor()

            # Fetch all games
            cursor.execute("SELECT GAME_NO, WHITE_HANDLE, BLACK_HANDLE, RESULT FROM games")
            games = cursor.fetchall()
            connection.close()

            # Generate HTML content
            content = """
            <!DOCTYPE html>
            <html>
            <head>
                <title>Game History</title>
            </head>
            <body>
                <h1>Game History</h1>
                <table border="1">
                    <tr>
                        <th>Game Number</th>
                        <th>White Player</th>
                        <th>Black Player</th>
                        <th>Result</th>
                    </tr>
            """
            for game_no, white_handle, black_handle, result in games:
                content += f"""
                <tr onclick="window.location.href='/gamelog.html?game_no={game_no}'">
                    <td>{game_no}</td>
                    <td>{white_handle}</td>
                    <td>{black_handle}</td>
                    <td>{result}</td>
                </tr>
                """

            content += """
                </table>
                <a href="/index.html">Return to Lobby</a>
            </body>
            </html>
            """

            # Send response
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.send_header("Content-length", len(content))
            self.end_headers()
            self.wfile.write(bytes(content, "utf-8"))

        elif parsed.path == '/gamelog.html':
            query_components = parse_qs(parsed.query)
            game_no = query_components.get('game_no', [None])[0]

            if game_no:
                connection = sqlite3.connect('hostage_chess.db')
                cursor = connection.cursor()

                # Fetch game boards and result
                cursor.execute("""
                    SELECT TURN_NO, TURN, BOARD, REAL_TIME 
                    FROM game_states 
                    WHERE GAME_NO = ? ORDER BY TURN_NO
                """, (game_no,))
                game_states = cursor.fetchall()

                cursor.execute("SELECT RESULT FROM games WHERE GAME_NO = ?", (game_no,))
                game_result = cursor.fetchone()[0]

                connection.close()

                # Generate HTML content
                content = f"""
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Game Log</title>
                    <link rel="stylesheet" href="css/chessboard-1.0.0.css">
                    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
                    <script src="js/chessboard-1.0.0.js"></script>
                </head>
                <body>
                    <h1>Game Log for Game {game_no}</h1>
                """

                for turn_no, turn, board, real_time in game_states:
                    content += f"""
                    <h3>Turn {turn_no}: { 'White' if turn == 'w' else 'Black' }</h3>
                    <p>Time: {real_time}</p>
                    <div id="board{turn_no}" style="width: 400px;"></div>
                    <script>
                        Chessboard('board{turn_no}', {{ position: '{board}', draggable: false }});
                    </script>
                    <hr>
                    """

                content += f"""
                    <h2>Game Result: {game_result}</h2>
                    <a href="/history.html">Back to Game History</a><br>
                    <a href="/index.html">Return to Lobby</a>
                </body>
                </html>
                """

                # Send response
                self.send_response(200)
                self.send_header("Content-type", "text/html")
                self.send_header("Content-length", len(content))
                self.end_headers()
                self.wfile.write(bytes(content, "utf-8"))

            else:
                self._send_response("400", status=400)

        elif parsed.path == '/over.html':
            # Parse query parameters to get the game number
            query_components = parse_qs(parsed.query)
            game_no = query_components.get('game_no', [None])[0]

            connection = sqlite3.connect('hostage_chess.db')
            cursor = connection.cursor()

            # Fetch the game result
            cursor.execute("SELECT RESULT FROM games WHERE GAME_NO = ?", (game_no,))
            result_row = cursor.fetchone()
            connection.close()

            game_result = result_row[0]
 
            # Generate the HTML content
            content = f"""
            <!DOCTYPE html>
            <html>
            <head>
                <title>Game Over</title>
            </head>
            <body>
                <h1>Game Over</h1>
                <p>Game {game_no}: {game_result}</p>
                <a href="/index.html">Return to Lobby</a>
            </body>
            </html>
            """

            # Send the HTTP response
            self.send_response(200)  # OK
            self.send_header("Content-type", "text/html")
            self.send_header("Content-length", len(content))
            self.end_headers()
            self.wfile.write(bytes(content, "utf-8"))


        elif parsed.path == '/css/chessboard-1.0.0.css':
            with open('.' + self.path, 'rb') as fp:
                content = fp.read()  
                
            self.send_response(200)  # OK
            # notice the change in Content-type
            self.send_header("Content-type", "text/css")
            self.send_header("Content-length", len(content))
            self.end_headers()

            self.wfile.write(content)  # binary file
        elif parsed.path.endswith('.png'):
            with open('.' + self.path, 'rb') as fp:
                content = fp.read()  
                
            self.send_response(200)  # OK
            # notice the change in Content-type
            self.send_header("Content-type", "image/png")
            self.send_header("Content-length", len(content))
            self.end_headers()

            self.wfile.write(content)  # binary file
        
        elif parsed.path == '/js/chessboard-1.0.0.js':
            with open('.' + self.path, 'rb') as fp:
                content = fp.read()  
                
            self.send_response(200)  # OK
            # notice the change in Content-type
            self.send_header("Content-type", "application/javascript")
            self.send_header("Content-length", len(content))
            self.end_headers()

            self.wfile.write(content)  # binary file
            

        elif parsed.path in ['/index.html', '/']:
            content = """
            <!DOCTYPE html>
            <html>
            <head>
                <title>Lobby</title>
            </head>
            <body>
                <h1>Chess Lobby</h1>
                <form action="login.html" method="post">
                    <label for="handle">Handle:</label>
                    <input type="text" id="handle" name="handle" required>
                    <button type="submit">Submit</button>
                </form>
                <p>Or visit the <a href="/history.html">game history</a>.</p>
            </body>
            </html>
            """
            self.send_response(200)  # OK
            self.send_header("Content-type", "text/html")
            self.send_header("Content-length", len(content))
            self.end_headers()
            self.wfile.write(bytes(content, "utf-8"))

        elif parsed.path == '/check_black_handle':
            
            query_components = parse_qs(parsed.query)
            game_no = query_components.get('game_no', [None])[0]
            if game_no:
                connection = sqlite3.connect('hostage_chess.db')
                cursor = connection.cursor()

                # Check BLACK_HANDLE
                cursor.execute("SELECT BLACK_HANDLE FROM games WHERE GAME_NO = ?", (game_no,))
                black_handle = cursor.fetchone()[0]

                connection.close()

                # If the opponent has joined, return their handle; otherwise, return "null"
                if black_handle:
                    self._send_response(black_handle, content_type="text/plain")
                else:
                    self._send_response("null", content_type="text/plain")
            else:
                self._send_response("400: Game number is required", status=400)

        else:			
            self.send_response(404)
            self.end_headers()
            self.wfile.write(bytes("404: %s not found" % self.path, "utf-8"))

    # Serve the POST requests
    def do_POST(self):
        parsed = urlparse(self.path)
        if parsed.path == '/login.html':
            form = cgi.FieldStorage(
                fp=self.rfile,
                headers=self.headers,
                environ={'REQUEST_METHOD': 'POST', 'CONTENT_TYPE': self.headers['Content-Type']}
            )
            handle = form.getvalue('handle')

            if not handle:
                self._send_response("400: Handle is required", status=400)
                return

            connection = sqlite3.connect('hostage_chess.db')
            cursor = connection.cursor()

            # Check for an open game with an empty BLACK_HANDLE
            cursor.execute("SELECT GAME_NO, BLACK_HANDLE FROM games WHERE BLACK_HANDLE IS NULL LIMIT 1")
            game_row = cursor.fetchone()

            if game_row:
                # Scenario 2: Join an existing game
                game_no = game_row[0]
                cursor.execute("UPDATE games SET BLACK_HANDLE = ? WHERE GAME_NO = ?", (handle, game_no))
                cursor.execute('''INSERT INTO game_states (GAME_NO, TURN_NO, TURN, BOARD, REAL_TIME, WHITE_TIME, BLACK_TIME)
                    VALUES (?, ?, ?, ?, ?, ?, ?)''', 
                (game_no, 1, 'w', "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", time.strftime('%Y-%m-%d %H:%M:%S', time.localtime()), GAME_TIME, GAME_TIME))
                connection.commit()

                # HTML response with JavaScript redirection
                content = f"""
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Game Ready</title>
                    <script>
                        setTimeout(function() {{
                            document.getElementById('redirectForm').submit();
                        }}, 1000); 
                    </script>
                </head>
                <body>
                    <h1>Game {game_no} is ready!</h1>
                    <p>You are Black. Redirecting you to your game...</p>
                    <form id="redirectForm" action="/opponent.html" method="POST" style="display:none;">
                        <input type="hidden" name="game_no" value="{game_no}">
                        <input type="hidden" name="turn_no" value="1">
                    </form>
                </body>
                </html>
                """
                self.send_response(200)  # OK
                self.send_header("Content-type", "text/html")
                self.send_header("Content-length", len(content))
                self.end_headers()
                self.wfile.write(bytes(content, "utf-8"))

            else:
                # Scenario 1: Start a new game
                cursor.execute("INSERT INTO games (WHITE_HANDLE, BLACK_HANDLE) VALUES (?, ?)", (handle, None))
                game_no = cursor.lastrowid
                connection.commit()

                # Periodically check if BLACK_HANDLE has been set (i.e., if opponent has joined)
                content = f"""
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Waiting for Opponent</title>
                    <script>
                        var game_no = {game_no};

                        setInterval(() => {{
                            fetch(`/check_black_handle?game_no=${{game_no}}`).then(response => {{
                                if (response.ok) {{
                                    response.text().then(data => {{
                                        if (data !== "null") {{
                                            var form = document.getElementById('redirectForm');
                                            form.submit();
                                        }}
                                    }});
                                }}
                            }});
                        }}, 1000);
                    </script>
                </head>
                <body>
                    <form id="redirectForm" action="/player.html" method="POST" style="display:none;">
                        <input type="hidden" name="game_no" value="{game_no}">
                        <input type="hidden" name="turn_no" value="1">
                    </form>
                </body>
                </html>
                """
                self.send_response(200)  # OK
                self.send_header("Content-type", "text/html")
                self.send_header("Content-length", len(content))
                self.end_headers()
                self.wfile.write(bytes(content, "utf-8"))

            connection.close()

        elif parsed.path == '/opponent.html':
            form = cgi.FieldStorage(
                fp=self.rfile,
                headers=self.headers,
                environ={'REQUEST_METHOD': 'POST', 'CONTENT_TYPE': self.headers['Content-Type']}
            )
            game_no = form.getvalue('game_no')
            turn_no = int(form.getvalue('turn_no'))  
            board = form.getvalue('board')
            turn = form.getvalue('turn')
            print(board)

            if not game_no:
                self._send_response("400:", status=400)
                return

            connection = sqlite3.connect('hostage_chess.db')
            cursor = connection.cursor()

            if not board:
                # If no board is provided, wait for the next turn
                next_turn_no = turn_no + 1
                # Render a waiting page that checks for the next turn
                white_time_minutes = GAME_TIME // 60
                white_time_seconds = GAME_TIME % 60
                black_time_minutes = GAME_TIME// 60
                black_time_seconds = GAME_TIME % 60
                content = f"""
                    <!DOCTYPE html>
                    <html>
                    <head>
                        <title>Waiting for Next Turn</title>
                        <link rel="stylesheet" href="css/chessboard-1.0.0.css">
                        <script src="http://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
                        <script src="js/chessboard-1.0.0.js"></script>

                        <script>
                            var game_no = {game_no};  // Ensure game_no is correctly formatted

                            setInterval(() => {{
                                fetch('/check_game_state', {{
                                    method: 'POST',
                                    headers: {{
                                        'Content-Type': 'application/x-www-form-urlencoded'  // Correct string format
                                    }},
                                    body: `game_no=${game_no}&turn_no=${next_turn_no}`  // Correct body format
                                }})
                                .then(response => {{
                                    if (response.ok) {{
                                        response.text().then(data => {{
                                            if (data == "ready") {{
                                                // Submit the form and redirect
                                                var form = document.getElementById('checkGameStateForm');
                                                form.submit();  // Submit the form to trigger a redirect
                                            }}else if (data == "end") {{
                                                    window.location.href = "/over.html?game_no={game_no}";
                                                }}                                          
                                        }});
                                    }}
                                }});
                            }}, 1000); 
                        </script>
                    </head>
                    <body>
                        <h1>Waiting for Opponents Move...</h1>
                        <div id="myBoard" style="width: 400px;"></div>
                        <div>White Time: <span id="whiteTime">{white_time_minutes}:{white_time_seconds:02}</span></div>
                        <div>Black Time: <span id="blackTime">{black_time_minutes}:{black_time_seconds:02}</span></div>
                        <!-- Hidden form for POST request -->
                        <form id="checkGameStateForm" action="/player.html" method="POST" style="display:none;">
                            <input type="hidden" name="game_no" value="{game_no}">
                            <input type="hidden" name="turn_no" value="{next_turn_no}">
                        </form>
                    <script>
                    // Initialize the chessboard
                    var board = Chessboard('myBoard', {{
                        position: 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR',
                        draggable: false
                    }});

                    var whiteTime = {GAME_TIME};

                    function updateClock() {{
                        if (whiteTime > 0) {{
                            whiteTime--;
                            var minutes = Math.floor(whiteTime / 60);
                            var seconds = whiteTime % 60;
                            document.getElementById('whiteTime').innerHTML = minutes + ':' + (seconds < 10 ? '0' + seconds : seconds);
                        }}
                    }}
                    setInterval(updateClock, 1000);
                </script>
                    </body>
                    </html>

                """
                self.send_response(200)
                self.send_header("Content-type", "text/html")
                self.send_header("Content-length", len(content))
                self.end_headers()
                self.wfile.write(bytes(content, "utf-8"))
                connection.close()

            # If board is provided, handle the move
            next_turn_no = turn_no + 1
            turn = 'b' if form.getvalue('turn') == 'w' else 'b'
            next_turn = form.getvalue('turn')
            current_time = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
                
                

            cursor.execute("SELECT TURN, REAL_TIME, WHITE_TIME, BLACK_TIME FROM game_states WHERE GAME_NO = ? AND TURN_NO = ?", (game_no, turn_no))
            game_state = cursor.fetchone()
            turn, previous_time, white_time, black_time = game_state

            # Calculate elapsed time
            elapsed_time = time.time() - time.mktime(time.strptime(previous_time, '%Y-%m-%d %H:%M:%S'))

            if turn == 'w':
                white_time -= int(elapsed_time)
            else:
                black_time -= int(elapsed_time)

            if white_time<=0 or black_time<=0:
                if white_time <= 0:
                    cursor.execute("""
                        UPDATE games
                        SET RESULT = ?
                        WHERE GAME_NO = ?
                    """, ("out of time white", game_no))
                else:
                    cursor.execute("""
                        UPDATE games
                        SET RESULT = ?
                        WHERE GAME_NO = ?
                    """, ("out of time black", game_no))
                connection.commit()  

                content = f"""
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Game Over</title>
                </head>
                <body>
                    <h1>Game Over</h1>
                    <p>{'White' if white_time <= 0 else 'Black'} lost on time.</p>
                    <a href="/index.html">Return to Lobby</a>
                </body>
                </html>
                """
                self.send_response(200) 
                self.send_header("Content-type", "text/html")
                self.send_header("Content-length", len(content))
                self.end_headers()
                self.wfile.write(bytes(content, "utf-8"))                               
                return
                
            # Insert the new game state
            cursor.execute("""
                INSERT INTO game_states (GAME_NO, TURN_NO, TURN, BOARD, REAL_TIME, WHITE_TIME, BLACK_TIME)
                VALUES (?, ?, ?, ?, ?, ?, ?)
            """, (game_no, next_turn_no, next_turn, board, current_time, white_time, black_time))


            white_time_minutes = white_time // 60
            white_time_seconds = white_time % 60
            black_time_minutes = black_time // 60
            black_time_seconds = black_time % 60
            
            connection.commit()
            connection.close()
            next_turn_no +=1
            print(game_no,next_turn_no)
            
            content = f"""
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Waiting for Next Turn</title>
                    <link rel="stylesheet" href="css/chessboard-1.0.0.css">
                    <script src="http://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
                    <script src="js/chessboard-1.0.0.js"></script>
                </head>
                <body>
                    <h1>Game {game_no} - Wait For Turn</h1>
                    <div id="myBoard" style="width: 400px;"></div>
                    <div>White Time: <span id="whiteTime">{white_time_minutes}:{white_time_seconds:02}</span></div>
                    <div>Black Time: <span id="blackTime">{black_time_minutes}:{black_time_seconds:02}</span></div>

                    <form id="checkGameStateForm" action="/player.html" method="POST">
                        <input type="hidden" name="game_no" value="{game_no}">
                        <input type="hidden" name="turn_no" value="{next_turn_no}">
                    </form>

                    <script>
                        // Initialize chessboard
                        Chessboard('myBoard', {{ position: '{board}', draggable: false }});
                    
                        const turn = '{next_turn}';
                        const game_no = {game_no};
                        const next_turn_no = {next_turn_no};

                        // Update the clock time
                        var whiteTime = {white_time};
                        var blackTime = {black_time};


                    // Function to update the clock
                    function updateClock() {{
                        if (whiteTime > 0 && 'w'== turn) {{
                            whiteTime--;
                            var minutes = Math.floor(whiteTime / 60);
                            var seconds = whiteTime % 60;
                            document.getElementById('whiteTime').innerHTML = minutes + ':' + (seconds < 10 ? '0' + seconds : seconds);
                        }}

                        if (blackTime > 0 && 'b' == turn) {{
                            blackTime--;
                            var minutes = Math.floor(blackTime / 60);
                            var seconds = blackTime % 60;
                            document.getElementById('blackTime').innerHTML = minutes + ':' + (seconds < 10 ? '0' + seconds : seconds);
                        }}
                    }}

                        // Check game state
                        function checkGameState() {{
                            fetch('/check_game_state', {{
                                method: 'POST',
                                headers: {{ 'Content-Type': 'application/x-www-form-urlencoded' }},
                                body: `game_no=${{game_no}}&turn_no=${{next_turn_no}}`
                            }})
                            .then(response => response.ok ? response.text() : Promise.reject('Failed to fetch game state'))
                            .then(data => {{
                                if (data === "ready") {{
                                    document.getElementById('checkGameStateForm').submit();
                                }} else if (data === "end") {{
                                    // Redirect to over.html
                                    window.location.href = "/over.html?game_no={game_no}";
                                }}
                            }})
                        }}

                        // Periodic updates
                        setInterval(updateClock, 1000);
                        setInterval(checkGameState, 1000);
                    </script>
                </body>
                </html>
                """

            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.send_header("Content-length", len(content))
            self.end_headers()
            self.wfile.write(bytes(content, "utf-8"))
            
        elif parsed.path == '/player.html':
            form = cgi.FieldStorage(
                fp=self.rfile,
                headers=self.headers,
                environ={'REQUEST_METHOD': 'POST', 'CONTENT_TYPE': self.headers['Content-Type']}
            )
            game_no = form.getvalue('game_no')
            turn_no = form.getvalue('turn_no')

            connection = sqlite3.connect('hostage_chess.db')
            cursor = connection.cursor()

            # Retrieve the current game state
            cursor.execute("SELECT TURN, BOARD, WHITE_TIME, BLACK_TIME FROM game_states WHERE GAME_NO = ? AND TURN_NO = ?", (game_no, turn_no))
            game_state = cursor.fetchone()
            turn, board, white_time, black_time = game_state
            if 'K' not in board or 'k' not in board:
                if 'K' not in board:
                    cursor.execute("""
                        UPDATE games
                        SET RESULT = ?
                        WHERE GAME_NO = ?
                    """, ("white king taken", game_no))
                else:
                    cursor.execute("""
                        UPDATE games
                        SET RESULT = ?
                        WHERE GAME_NO = ?
                    """, ("black king taken", game_no))
                connection.commit()

                cursor.execute("SELECT RESULT FROM games WHERE GAME_NO = ?", (game_no,))
                result_row = cursor.fetchone()
                connection.close()

                game_result = result_row[0] or "No result recorded"
                content = f"""
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Game Over</title>
                </head>
                <body>
                    <h1>Game Over</h1>
                    <p>Game {game_no}: {game_result}</p>
                    <a href="/index.html">Return to Lobby</a>
                </body>
                </html>
                """
                
                # Send the HTTP response
                self.send_response(200)  # OK
                self.send_header("Content-type", "text/html")
                self.send_header("Content-length", len(content))
                self.end_headers()
                self.wfile.write(bytes(content, "utf-8"))

                return

            next_turn = 'b' if turn == 'w' else 'w'
            # Convert time to minutes and seconds format
            white_time_minutes = white_time // 60
            white_time_seconds = white_time % 60
            black_time_minutes = black_time // 60
            black_time_seconds = black_time % 60
            # Render the player's board
            content = f"""
            <!DOCTYPE html>
            <html>
            <head>
                <title>Player Turn</title>
                <link rel="stylesheet" href="css/chessboard-1.0.0.css">
            </head>
            <body>
                <h1>Game {game_no} - Your Turn</h1>
                <div id="myBoard" style="width: 400px;"></div>
                <div>White Time: <span id="whiteTime">{white_time_minutes}:{white_time_seconds:02}</span></div>
                <div>Black Time: <span id="blackTime">{black_time_minutes}:{black_time_seconds:02}</span></div>
                <form action="/opponent.html" method="post" onsubmit="submitBoard();">
                    <input type="hidden" name="game_no" value="{game_no}">
                    <input type="hidden" name="turn_no" value="{turn_no}">
                    <input type="hidden" name="board" id="board">
                    <input type="hidden" name="turn" value="{next_turn}">
                    <button type="submit">Done</button>
                </form>
                <script src="http://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
                <script src="js/chessboard-1.0.0.js"></script>
                <script>
                    var board = Chessboard('myBoard', {{
                        position: '{board}',
                        draggable: true
                    }});

                    function submitBoard() {{
                        document.getElementById('board').value = board.fen();
                    }}

                    var whiteTime = {white_time};
                    var blackTime = {black_time};
                    var turn = '{turn}';

                    // Function to update the clock
                    function updateClock() {{
                        if (whiteTime > 0 && 'w'== turn) {{
                            whiteTime--;
                            var minutes = Math.floor(whiteTime / 60);
                            var seconds = whiteTime % 60;
                            document.getElementById('whiteTime').innerHTML = minutes + ':' + (seconds < 10 ? '0' + seconds : seconds);
                        }}

                        if (blackTime > 0 && 'b' == turn) {{
                            blackTime--;
                            var minutes = Math.floor(blackTime / 60);
                            var seconds = blackTime % 60;
                            document.getElementById('blackTime').innerHTML = minutes + ':' + (seconds < 10 ? '0' + seconds : seconds);
                        }}
                    }}

                    // Update the clock every second
                    setInterval(updateClock, 1000);
                </script>
            </body>
            </html>
            """

            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.send_header("Content-length", len(content))
            self.end_headers()
            self.wfile.write(bytes(content, "utf-8"))
       
        elif parsed.path == '/check_game_state':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length).decode('utf-8')

            # Parse the POST data
            post_params = parse_qs(post_data)
            game_no = post_params.get('game_no', [None])[0]
            turn_no = post_params.get('turn_no', [None])[0]

            # Sanitize inputs
            game_no = game_no.strip().lstrip('$') if game_no else None
            turn_no = turn_no.strip().lstrip('$') if turn_no else None
    
            connection = sqlite3.connect('hostage_chess.db')
            cursor = connection.cursor()
            
            cursor.execute("SELECT RESULT FROM games WHERE GAME_NO = ?", (game_no,))
            result_row = cursor.fetchone()
            print(result_row)
            if result_row:
                game_result = result_row[0]
                if game_result =="out of time white" or game_result =="out of time black" or game_result =="white king taken" or game_result =="black king taken":
                    print(game_result)
                    # Send back the result if the game has ended
                    self._send_response("end", content_type="text/plain")

            # Check if a row exists with the requested turn_no
            cursor.execute("SELECT 1 FROM game_states WHERE GAME_NO = ? AND TURN_NO = ?", (game_no, int(turn_no)))
            result = cursor.fetchone()
            connection.close()
            if result:
                self._send_response("ready", content_type="text/plain")
            else:
                self._send_response("not ready", content_type="text/plain")


        else:
            self._send_response(f"404: {self.path} not found", status=404)

    def _send_response(self, content, status=200, content_type="text/plain"):
        self.send_response(status)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", len(content))
        self.end_headers()
        self.wfile.write(bytes(content, "utf-8"))

# Main server setup
if __name__ == "__main__":
    initialize_db()
    server_address = ('0.0.0.0', int(sys.argv[1]))
    httpd = HTTPServer(server_address, MyHandler)
    print(f"Server listening on port {sys.argv[1]}")
    httpd.serve_forever()
