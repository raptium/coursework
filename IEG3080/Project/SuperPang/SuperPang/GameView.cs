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

            GraphicManager = BufferedGraphicsManager.Current;
            GraphicManager.MaximumBuffer =  new Size(this.Width + 1, this.Height + 1);
            ManagedBackBuffer = GraphicManager.Allocate(this.CreateGraphics(), ClientRectangle);
            ManagedBackBuffer.Graphics.SmoothingMode = SmoothingMode.HighQuality;
            ManagedBackBuffer.Graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;

        }


        public override void update(Model model)
        {
            foreach (GameObject obj in model.gameObjects)
            {
                obj.move(800 - obj.Position.Width, 500 - 22 - obj.Position.Height);
            }
            Invalidate();
        }

        private void makeObject(Graphics ControlGraphics, GameObject obj)
        {
            Bitmap img = Properties.Resources.Hero;
            try
            {
                ControlGraphics.DrawImage(obj.Image, obj.Position.X, obj.Position.Y, obj.Position.Width, obj.Position.Height);
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e);
            }
        }

        private void drawBackgroud(Graphics ControlGraphics)
        {
            ControlGraphics.Clear(Color.Empty);
            ControlGraphics.DrawImage(Properties.Resources.backgroud, 0, 0, 800, 500);
        }

        private void drawObjects(Graphics ControlGraphics)
        {
            foreach (GameObject obj in model.gameObjects)
            {
                makeObject(ControlGraphics, obj);
            }
        }
    }
}
