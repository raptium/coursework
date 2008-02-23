using System;
using System.Drawing;
using System.Threading;
using System.Collections;
using System.Diagnostics;

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
        private Ball myBall;
        //void move(); //put the event into the method...
        //void turnLeft(); //put the event into the method¡K
        //View createView();
        //void createController();

        // timer object to update myself
        private Timer timer;
        private bool moving = true;


        // reference back to the original form
        private MainForm form = null;


        public Model(MainForm form)
        {
            this.form = form;
            initGame();
            TimerCallback timerDelegate = new TimerCallback(allMove);
            this.timer = new Timer(timerDelegate, null, 500, 40);
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
            //currentView.BackgroundImage = Properties.Resources.backgroud;
            form.switchView(currentView);
        }

        public void initGame()
        {
            Rectangle heroPos = new Rectangle(form.Width / 2, form.Height - 180, 20, 30);
            Rectangle ballPos = new Rectangle(form.Width / 2, form.Height - 400, 20, 30);
            myHero = new Hero(heroPos, 0, 0, 0, this);
            myBall = new Ball(ballPos, 0, -15, 0, this);
        }

        public Hero getHero()
        {
            return myHero;
        }

        public Ball getBall()
        {
            return myBall;
        }

        private void allMove(object stateInfo)
        {
            if (moving == true)
            {
                // make update on the position of all elements in game scene

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

    }
}
