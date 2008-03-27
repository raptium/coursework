using System;
using System.Drawing;
using System.Threading;
using System.Collections;
using System.Diagnostics;
using System.Collections.Generic;
using System.Windows.Forms;

namespace SuperPang
{
    public class Model
    {
        // event for the communciation between model and view
        public delegate void ViewUpdater(Model model);
        public event ViewUpdater ModelUpdateEvent;
        //declare the view
        private View currentView;
        //declare the controller
        private Controller currentController;
        private Hero myHero;
        //private Ball myBall;
        private List<Ball> balls;
        private List<Bullet> bullets;

        // timer object to update myself
        private System.Threading.Timer timer;
        private int life;
        private int shield;
        private Boolean win;


        // reference back to the original form
        private MainForm form = null;


        public Model(MainForm form)
        {
            this.form = form;
            initGame();
            TimerCallback timerDelegate = new TimerCallback(allMove);
            this.timer = new System.Threading.Timer(timerDelegate, null, 500, 40);
            
        }

        public void createController()
        {
            this.currentController = new Controller(this);

            // register the key event handler to controller
            form.KeyUp += new System.Windows.Forms.KeyEventHandler(currentController.keyUpHandler);
            form.KeyDown += new System.Windows.Forms.KeyEventHandler(currentController.keyDownHandler);
        }

        public void createView()
        {
            currentView = new GameView(this, currentController);

            currentView.SetBounds(0, 0, 800, 500);
            form.switchView(currentView);
        }

        public void initGame()
        {
            Rectangle heroPos = new Rectangle(form.Width / 2, form.Height - 100, 35, 100);
            Rectangle ballPos = new Rectangle(form.Width / 2, form.Height - 400, 96, 96);
            myHero = new Hero(heroPos, 0, 0, this);
            balls = new List<Ball>();
            bullets = new List<Bullet>();
            balls.Add(new Ball(ballPos, -10, 0, this));
            life = 3;
            win = false;
            shield = 0;
        }

        public List<GameObject> gameObjects
        {
            get { 
                List<GameObject> objs = new List<GameObject>();
                foreach (Ball obj in balls)
                {
                    objs.Add((GameObject)obj);
                }
                foreach (Bullet obj in bullets)
                {
                    objs.Add((GameObject)obj);
                }
                objs.Add(myHero);

                for (int i = 0; i < life; i++)
                {
                    Rectangle pos = new Rectangle(10 + 20 * i, 10, 10, 30);
                    objs.Add(new Misc(pos, 0, 0, this, Properties.Resources.Hero));
                }

                if (life == 0)
                {
                    Rectangle pos = new Rectangle(150, 100, 500, 300);
                    objs.Add(new Misc(pos, 0, 0, this, Properties.Resources.gameover));
                }
                else if (win)
                {
                    Rectangle pos = new Rectangle(150, 100, 500, 300);
                    objs.Add(new Misc(pos, 0, 0, this, Properties.Resources.win));
                }


                return objs;
            }
        }

        private void allMove(object stateInfo)
        {
            if (life > 0 && !win)
            {
                // make update on the position of all elements in game scene
                int i, j;

                i = 0;
                while (i < balls.Count)
                {
                    Ball ball = balls[i];
                    if (isCollision(ball, myHero) && shield == 0)
                    {
                        life--;
                        shield = 30;
                        break;
                    }

                    if (shield > 0)
                        shield--;

                    j = 0;
                    while (j < bullets.Count)
                    {
                        Bullet b = bullets[j];
                        if (!ball.Destroyed && !b.Destroyed && isCollision(ball, b))
                        {
                            balls.AddRange(ball.Split());
                            balls.Remove(ball);
                            b.Destroyed = true;
                        }
                        j++;
                    }

                    i++;
                }

                i = 0;
                while (i < bullets.Count)
                {
                    Bullet b = bullets[i];

                    if (b.Destroyed)
                    {
                        bullets.Remove(b);
                    }

                    i++;
                }

                if (balls.Count == 0)
                    win = true;

                // fire the change event to notify the view
                ModelUpdateEvent(this);
            }
        }

        public void moveHero(int direct)
        {
            if (direct == -1)
                myHero.moveLeft();
            else if (direct == 1)
                myHero.moveRight();
        }

        public void stopHero()
        {
            myHero.stop();
        }

        public void jumpHero()
        {
            myHero.jump();
        }

        private Boolean isCollision(GameObject a, GameObject b)
        {
            return a.Position.IntersectsWith(b.Position);
        }

        public void Fire()
        {
            if (bullets.Count >= 3)
                return;
            Rectangle bpos;
            bpos = myHero.Position;
            bpos.Width = 12;
            bpos.Height = 12;
            Bullet b = new Bullet(bpos, 0, -10, this);
            bullets.Add(b);
        }

        public void Reset()
        {
            Rectangle heroPos = new Rectangle(form.Width / 2, form.Height - 100, 35, 100);
            Rectangle ballPos = new Rectangle(form.Width / 2, form.Height - 400, 96, 96);
            myHero = new Hero(heroPos, 0, 0, this);
            balls.Clear();
            bullets.Clear();
            balls.Add(new Ball(ballPos, -10, 0, this));
            life = 3;
            win = false;
            shield = 0;
        }
    }
}
