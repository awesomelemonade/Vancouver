from Color import Color
from TetrisBoard import TetrisBoard
import time, random


class TetrisGame:
	def __init__(self):
		self.BLOCK_SIZE = 20
		self.close_requested = False
		self.target_fps = 60
		self.window_size = (3 * 10 * self.BLOCK_SIZE, 1 * 22 * self.BLOCK_SIZE)
		self.title = "Tetris"
	def start(self, listener):
		# Init
		self.screen = pygame.display.set_mode(self.window_size)
		pygame.display.set_caption(self.title)
		self.clock = pygame.time.Clock()
		board = TetrisBoard(10, 20, seed=0)
		lastUpdate = time.time()
		environment = None
		leftTime = 0
		rightTime = 0
		verTime = 0
		upTime = 0
		UPDATE_INTERVAL = 1
		HORIZONTAL_INTERVAL = 0.1
#DOWN_INTERVAL = 0.05
		DOWN_INTERVAL = 0.33
		ROTATIONAL_INTERVAL = 0.33

		up = False
		left = False
		right = False
		down = False
		# Main Game Loop
		while not self.close_requested:
			# Handles "X" button of window
			spiked = False
			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					self.close_requested = True
				#press button
				if environment is None:
					if event.type == pygame.KEYDOWN:
						if event.key == pygame.K_UP:
							if(not up): #inital press
								upTime = time.time()
								board.rotateFallingPiece(1)#for tapping the button vs holding it
							up = True
						if event.key == pygame.K_LEFT:
							if(not left): #inital press
								leftTime = time.time()
								board.moveFallingPiece(moveX=-1, moveY=0)#for tapping the button vs holding it
							left = True
						if event.key == pygame.K_RIGHT:
							if(not right):#inital press
								rightTime = time.time()
								board.moveFallingPiece(moveX=1, moveY=0) #for tapping the button vs holding it
							right = True
						if event.key == pygame.K_DOWN:
							if(not down):#initial press
								verTime = time.time()
								board.moveFallingPiece(moveX=0, moveY=1)#for tapping the button vs holding it
							down = True
						if event.key == pygame.K_SPACE:
							board.spike()
							spiked = True
						if event.key == pygame.K_ESCAPE:
							self.close_requested = True
						#release button
					elif event.type == pygame.KEYUP:
						if event.key == pygame.K_UP:
							up = False
						if event.key == pygame.K_LEFT:
							left = False
						if event.key == pygame.K_RIGHT:
							right = False
						if event.key == pygame.K_DOWN:
							down = False
			#execute hold
			if environment is None:
				numFingers = listener.getNumFingersExtended() #print(numFingers)
				movement = listener.getMovement()
				print(numFingers, movement)
				if up or numFingers == -1:
					if upTime + ROTATIONAL_INTERVAL < time.time():#rotates every 1/3 second
						board.rotateFallingPiece(1)
						upTime = time.time()
				if left or movement == -1:
					if leftTime + HORIZONTAL_INTERVAL < time.time():#moves left every .1 seconds
						board.moveFallingPiece(moveX=-1, moveY=0)
						leftTime = time.time()
				if right or movement == 1:
					if rightTime + HORIZONTAL_INTERVAL < time.time():#mvoes right every .1 seconds
						board.moveFallingPiece(moveX=1, moveY=0)
						rightTime = time.time()
				if down or numFingers == 1:
					if verTime + DOWN_INTERVAL < time.time():#moves down every .01 seconds
						board.rotateFallingPiece(3)
#board.moveFallingPiece(moveX=0, moveY=1)
						verTime = time.time()
			# Updates the game
			if lastUpdate + UPDATE_INTERVAL < time.time():
				if not environment is None:
					environment.step(board)
				if board.update() == -1: # Loss
					if not environment is None:
						environment.gameover()
					break
				lastUpdate += UPDATE_INTERVAL
			# Render the Board
			self.screen.fill((255,239,213))
			board.render(self.screen, *self.window_size)
			
			pygame.display.flip()
			self.clock.tick(self.target_fps)
import main
listener = main.test()
import pygame
pygame.init()
game = TetrisGame()
game.start(listener)
pygame.quit()
