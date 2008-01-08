using System;
using System.Windows.Forms;

#if STRONG
[assembly: System.Reflection.AssemblyVersion("1.0.0.0")]
[assembly: System.Reflection.AssemblyKeyFile("myKey.snk")]
#endif
public class HelloMsg{
    public void Write(){
        MessageBox.Show("Hello c# world");
    }
}
