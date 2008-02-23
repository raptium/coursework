using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.ComponentModel;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    class GameView : View
    {

        const BufferedGraphics NO_MANAGED_BACK_BUFFER = null;
        BufferedGraphicsContext GraphicManager;
        BufferedGraphics ManagedBackBuffer;

        private void MemoryCleanup(object sender, EventArgs e)
        {
            // clean up the memory

            if (ManagedBackBuffer != NO_MANAGED_BACK_BUFFER)
                ManagedBackBuffer.Dispose();
        }

        protected override void OnPaint(PaintEventArgs pe)
        {
            // we draw the progressbar into the image in the memory
            drawBackgroud(ManagedBackBuffer.Graphics);
            drawObjects(ManagedBackBuffer.Graphics);

            // now we draw the image into the screen
            
            ManagedBackBuffer.Render(pe.Graphics);
        }

        public GameView(Model model, Controller controller)
            : base(model, controller)
        {
            
            this.SetStyle(ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint |
                ControlStyles.OptimizedDoubleBuffer,
                true);
            

            Application.ApplicationExit += new EventHandler(MemoryCleanup);
            //SetStyle(ControlStyles.AllPaintingInWmPaint, true);

            GraphicManager = BufferedGraphicsManager.Current;
            GraphicManager.MaximumBuffer =  new Size(this.Width + 1, this.Height + 1);
            ManagedBackBuffer = GraphicManager.Allocate(this.CreateGraphics(), ClientRectangle);
            ManagedBackBuffer.Graphics.SmoothingMode = SmoothingMode.HighQuality;
            ManagedBackBuffer.Graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;
            //g.CompositingMode = CompositingMode.SourceCopy;

        }


        public override void update(Model model)
        {
            Invalidate();
            model.getHero().move(800 - 50, 500 - 160);
            model.getBall().move(800 - 96, 500 - 110);
            //makeHero(model.getHero());
            //makeBall(model.getBall());
        }

        private void makeHero(Graphics ControlGraphics, Hero hero)
        {
            Bitmap img = Properties.Resources.Hero;
            try
            {
                ControlGraphics.DrawImage(img, hero.getLocationInfo().X, hero.getLocationInfo().Y);
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e);
            }
        }

        private void makeBall(Graphics ControlGraphics, Ball ball)
        {
            Bitmap img = Properties.Resources.MacBook_White;
            ControlGraphics.DrawImage(img, ball.getLocationInfo().X, ball.getLocationInfo().Y, 96, 96);
        }

        private void drawBackgroud(Graphics ControlGraphics)
        {
            ControlGraphics.Clear(Color.Empty);
            ControlGraphics.DrawImage(Properties.Resources.backgroud, 0, 0, 800, 500);
        }

        private void drawObjects(Graphics ControlGraphics)
        {
            makeHero(ControlGraphics, model.getHero());
            makeBall(ControlGraphics, model.getBall());
        }
    }
}
