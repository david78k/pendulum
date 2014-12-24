# This code is so you can run the samples without installing the package
import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../..'))
#

import random, cPickle
import math

from pyglet import window
from pyglet import clock
from pyglet.gl import *

import pymunk._chipmunk as cp
import pymunk.util as u
from pymunk import Vec2d as vec2d
import ctypes

from numpy import arange, array, size
import numpy

import primitives as draw


INFINITY = float("infinity")
BACKGROUND = cp.cpBodyNew(INFINITY, INFINITY)

import reply

def cpvrotate(v1, v2):
    return vec2d(v1.x*v2.x - v1.y*v2.y, v1.x*v2.y + v1.y*v2.x)
def cpvadd(v1, v2):
    return vec2d(v1.x + v2.x, v1.y + v2.y)
dt = 1.0/30.0


class Ball:
    def __init__(self, space, mass, radius, pos=(300,300), color=(0,240,0,255)):
        self.color = color
        self.mass = mass
        self.radius = radius
        ball_moment = cp.cpMomentForCircle(mass, 5, 0.0, vec2d(0,0))
        self.body = cp.cpBodyNew( mass, ball_moment )
        self.shape = cp.cpCircleShapeNew(self.body,
                radius, vec2d(0.0, 0.0) )
        self.shape.contents.e = 0.5
        self.set_position( pos )
        cp.cpSpaceAddBody(space, self.body)
        cp.cpSpaceAddShape( space, self.shape )

    def set_position(self, position):
        self.body.contents.p = vec2d(position[0], position[1])

    def render(self):
        p = self.body.contents.p
        draw.circle((p[0]%800-800, p[1]), self.radius ,self.color)
        draw.circle((p[0]%800, p[1]), self.radius ,self.color)
        draw.circle((p[0]%800+800, p[1]), self.radius ,self.color)


    def __str__(self):
        return "<ball %s>"%self.body.contents.p

class Wall:
    def __init__(self, space, start, end, color=(0,255,255,255), radius=50):
        self.color = color
        self.start = start
        self.end = end
        self.radius = radius
        self.shape = cp.cpSegmentShapeNew(BACKGROUND,
            vec2d(*start), vec2d(*end), radius)
        self.shape.contents.e = 0.1
        self.shape.contents.u = 0.2
        cp.cpSpaceAddStaticShape(space, self.shape)

    def render(self):
        draw.rectangle(
            self.start[0], self.start[1],
            self.end[0], self.end[1]+self.radius
            ,self.color)

class Box:
    def __init__(self, space, mass, width, height, pos, color=(255,0,0,255)):
        poly = [
            [ width/2, height/2 ],
            [ width/2, -height/2 ],
            [ -width/2, -height/2 ],
            [ -width/2, height/2 ],
            ]
        p_num = len(poly)
        P_ARR = vec2d * p_num
        p_arr = P_ARR(vec2d(0,0))
        for i, (x,y) in enumerate( poly ):
            p_arr[i].x = x
            p_arr[i].y = y

        moment = cp.cpMomentForPoly( mass, p_num, p_arr, vec2d(0,0))
        self.color = color
        self.body = cp.cpBodyNew( mass, moment )
        self.body.contents.p = vec2d( *pos )
        cp.cpSpaceAddBody( space, self.body )
        self.shape = shape = cp.cpPolyShapeNew(
            self.body, p_num, p_arr, vec2d(0,0)
            )
        shape.contents.u = 0.5
        cp.cpSpaceAddShape(space, shape)


    def render(self):
        p = self.body.contents.p
        body = self.shape.contents.body
        shape = ctypes.cast(self.shape, ctypes.POINTER(cp.cpPolyShape))
        num = shape.contents.numVerts
        verts = shape.contents.verts
        ps = [
            cpvadd(body.contents.p,
                cpvrotate(verts[i], body.contents.rot))
            for i in range(num)]
        avg = (ps[0][0]+ps[1][0])/2
        points = []
        [ points.extend([p[0]-avg+avg%800, p[1]]) for p in ps ]
        pyglet.graphics.draw(4, GL_QUADS,
            ("v2f", points),
            ("c4B", list(self.color)*4)
            )
	glTranslatef(-800,0,0)
        pyglet.graphics.draw(4, GL_QUADS,
            ("v2f", points),
            ("c4B", list(self.color)*4)
            )
	glTranslatef(1600,0,0)
        pyglet.graphics.draw(4, GL_QUADS,
            ("v2f", points),
            ("c4B", list(self.color)*4)
            )
	glTranslatef(-800,0,0)


    def __str__(self):
        return "<box %s>"%self.body.contents.p


class Pendulum(reply.World):

    def __init__(self, filename):
        alpha = 0.3
        gamma = 0.5
        alpha_decay = 0.999
        min_alpha = 0.005
        epsilon = 0.99
        epsilon_decay = 0.99
        min_epsilon = 0.001
        self.filename = filename
        try:
            pend = cPickle.load(open(filename))
        except IOError:
            e = reply.encoder.DistanceEncoder(
                self.get_state_space(), self.get_action_space()
                )
            pend = reply.RL(
                    reply.learner.QLearner(alpha, gamma, alpha_decay, min_alpha),
                    reply.storage.DebugTableStorage(e),
                    e,
                    reply.selector.EGreedySelector(epsilon, epsilon_decay, min_epsilon)
                )
        self.rl = pend
        self.win = window.Window(width=800, height=600)
        self.figure = None
        cp.cpInitChipmunk()

        #PID stuff
        self.integral = 0
        self.last_error = 0
        self.ki = 0.5
        self.kd = -0.5
        self.kp = 1.0



    def get_action_space(self):
        return [reply.dimension(-1,1,17)]

    def get_state_space(self):
        return [
            # ball angle
            reply.dimension(-math.pi/2,math.pi/2,11),
            # ball angular velocity
            reply.dimension(-math.pi/16,math.pi/16,11),
            # base velocity
            reply.dimension(-100,100,11),
        ]
    def get_reward(self, s ):
        if self.ball.body.contents.p.y < 100:
            return -1000
	if abs(self.ball_angle()) < math.pi/16:
            return math.pi/16 - abs(self.ball_angle())
        return 0
    
    def is_final(self, state):
        if self.ball.body.contents.p.y < 100:
            return True
        return False

    def ball_angle(self):
        x = self.ball.body.contents.p.x
        y = self.ball.body.contents.p.y
        cx = self.base.body.contents.p.x
        cy = self.base.body.contents.p.y

        x = x-cx
        y = y-cy
        if y == 0: return 0
        return math.atan( x/y )

    def save_angle(self):
        self.last_angle = self.ball_angle()

    def get_state(self):
        return [
            self.ball_angle(),
            self.ball_angle() - self.last_angle,
	    self.base.body.contents.v.x
            ]


    def do_action(self, solver, action):
        action = action[0]
	self.save_angle()
        cp.cpBodyApplyForce(self.base.body, vec2d(action*15000,0), vec2d(0,0))
        cp.cpSpaceStep(self.space, dt)
        cp.cpBodyResetForces(self.base.body)

        clock.tick()
        self.win.dispatch_events()

        self.win.clear()

        self.ball.render()
        self.floor.render()
        self.base.render()

        self.win.flip()


    def get_initial_state(self):
        # return first state
        self.space = space = cp.cpSpaceNew()
        space.contents.gravity = vec2d(0.0, -900.0)

	space.contents.damping = 0.01
        cp.cpSpaceResizeStaticHash(space, 50.0, 200)
        cp.cpSpaceResizeActiveHash(space, 50.0, 10)

        self.ball = ball = Ball(space, 5, 10, pos=(310, 180))
        self.floor = Wall(space, (-60000,40), (60000,40))
        self.base = base = Box(space, 5, 50, 20, (300,100))
        cp.cpResetShapeIdCounter()
        joint = cp.cpPinJointNew(
            ball.body, base.body, vec2d(0,0), vec2d(0,0)
            )
	self.save_angle()
        cp.cpSpaceAddJoint(space, joint)
        if self.rl.episodes%100==0:
            self.save()
        return self.get_state()

    def save(self):
        cPickle.dump(self.rl, open(self.filename, "w"))

    def cleanUp(self):
        cp.cpSpaceFree( self.space )

    def learn(self, iterations, max_steps=10000):
	avg_steps = 0
        try:
            while self.rl.episodes < iterations:
                self.rl.new_episode(self)
                for step in range(max_steps):
                    if not self.rl.step(self):
                        break
		avg_steps = avg_steps*0.95 + 0.05*step
                print ('Espisode: ',self.rl.episodes,'  Steps:',step,
                        ' Reward:',self.rl.total_reward,
                        'alpha', self.rl.learner.alpha,
                        'epsilon', self.rl.selector.epsilon,
			'average steps', avg_steps)
	finally:
            self.save()

def run(maxepisodes):
    p = Pendulum("pendulum.pkl")
    p.learn(maxepisodes)


if __name__ == '__main__':
    run(1000000)


