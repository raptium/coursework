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
        public event IEG3080.DrawObjectCreatedHandler ModelUpdateEvent;
        //declare the view
        private IEG3080.View currentView;
        //declare the controller
        private Controller currentController;
        private Hero myHero;
        //private Ball myBall;
        private List<Ball> balls;
        private List<Bullet> bullets;

        // timer object to update myself
        private int life;
        private int shield;
        private Boolean win;


       

        public Model()
        {
        
            currentView = new IEG3080.View();
            createController();
            initGame();

            //TimerCallback timerDelegate = new TimerCallback(allMove);
            //this.timer = new System.Threading.Timer(timerDelegate, null, 500, 40);

            currentView.TimerEvent += new IEG3080.TimerHandler(allMove);
            ModelUpdateEvent += new IEG3080.DrawObjectCreatedHandler(currentView.UpdateView);
            currentView.WindowClosed += new EventHandler(Exit);
            //currentView.ClientRectangle = new Rectangle(0, 0, 800, 500);
            //currentView.client
            
            currentView.Show();
            Application.Run();
            
        }

        void Exit(object sender, EventArgs e)
        {
            Application.Exit();
        }

        public void createController()
        {
            this.currentController = new Controller(this);

            // register the key event handler to controller
            //currentView.KeyUp += new System.Windows.Forms.KeyEventHandler(currentController.keyUpHandler);
            currentView.KeyDown += new System.Windows.Forms.KeyEventHandler(currentController.keyDownHandler);
        }

        public void createView()
        {
            
        }

        public void initGame()
        {
            Rectangle heroPos = new Rectangle(currentView.ClientRectangle.Width / 2, currentView.ClientRectangle.Height, 35, 100);
            Rectangle ballPos = new Rectangle(currentView.ClientRectangle.Width / 2, currentView.ClientRectangle.Height - 400, 96, 96);
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

                Rectangle pos = new Rectangle((currentView.ClientRectangle.Height - 800)/2, (currentView.ClientRectangle.Height + 500) / 2, 800, 500);
                objs.Add(new Misc(pos, 0, 0, this, Properties.Resources.gameover, "Resources\\backgroud.jpg"));

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
                    pos = new Rectangle(10 + 20 * i, 30, 10, 30);
                    objs.Add(new Misc(pos, 0, 0, this, Properties.Resources.Hero, "Resources\\Hero.png"));
                }

                if (life == 0)
                {
                    pos = new Rectangle((currentView.ClientRectangle.Width - 500) / 2, (currentView.ClientRectangle.Height + 300) / 2, 500, 300);
                    objs.Add(new Misc(pos, 0, 0, this, Properties.Resources.gameover, "Resources\\gameover.png"));
                }
                else if (win)
                {
                    pos = new Rectangle((currentView.ClientRectangle.Width - 500) / 2, (currentView.ClientRectangle.Height + 300) / 2, 500, 300);
                    objs.Add(new Misc(pos, 0, 0, this, Properties.Resources.win, "Resources\\win.png"));
                }


                return objs;
            }
        }

        private void allMove(object sender, EventArgs e)
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
                        balls.AddRange(ball.Split());
                        balls.Remove(ball);
                        shield = 15;
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
                foreach (GameObject obj in this.gameObjects)
                {    
                    obj.move(currentView.ClientRectangle.Width - obj.Position.Width, currentView.ClientRectangle.Height);
                }
            }
            foreach (GameObject obj in this.gameObjects)
            {
                IEG3080.DrawObjectEventArgs objArg = new IEG3080.DrawObjectEventArgs(obj.ImageSrc, obj.Position.X, obj.Position.Y, obj.Position.Width, obj.Position.Height);
                ModelUpdateEvent(this, objArg);
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
            Rectangle posA, posB;

            posA = a.Position;
            posB = b.Position;

            posA.Y -= posA.Height;
            posB.Y -= posB.Height;

            return posA.IntersectsWith(posB);
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
            Rectangle heroPos = new Rectangle(currentView.ClientRectangle.Width / 2, currentView.ClientRectangle.Height, 35, 100);
            Rectangle ballPos = new Rectangle(currentView.ClientRectangle.Width / 2, currentView.ClientRectangle.Height - 400, 96, 96);
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
