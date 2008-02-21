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
        //void move(); //put the event into the method...
        //void turnLeft(); //put the event into the method�K
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
            this.timer = new Timer(timerDelegate, null, 500, 50);
        }

        public void createController()
        {
            this.currentController = new Controller(this);

            // register the key event handler to controller
           // form.KeyUp += new System.Windows.Forms.KeyEventHandler(controller.keyHandler);
            //form.KeyDown += new System.Windows.Forms.KeyEventHandler(controller.keyHandler2);
        }

        public void createView()
        {
            currentView = new GameView(this, currentController);

            currentView.SetBounds(0, 0, 1024, 680);
            currentView.BackgroundImage = Properties.Resources.backgroud;
            form.switchView(currentView);

        }

        public void initGame()
        {
            Rectangle heroPos = new Rectangle(form.Width / 2, form.Height / 2, 20, 30);
            myHero = new Hero(heroPos, 0, 0, 0, this);
        }

        public Hero getHero()
        {
            return myHero;
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


    }
}