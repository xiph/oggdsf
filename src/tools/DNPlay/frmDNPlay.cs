using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using illiminable.libDSPlayDotNET;

namespace DNPlay
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class frmDNPlay : System.Windows.Forms.Form
	{
		private System.Windows.Forms.MainMenu mainMenu1;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem2;
		private System.Windows.Forms.MenuItem menuItem3;
		private System.Windows.Forms.MenuItem menuItem4;
		private System.Windows.Forms.MenuItem menuItem5;
		private System.Windows.Forms.MenuItem menuItem6;
		private System.Windows.Forms.MenuItem menuItem7;
		private System.Windows.Forms.Button cmdPlay;
		private System.Windows.Forms.TextBox txtFilename;
		private System.Windows.Forms.Button cmdLoad;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		//My Variables...
		protected DSPlay mPlayer;
		//

		public frmDNPlay()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			mPlayer = new DSPlay();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.mainMenu1 = new System.Windows.Forms.MainMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.menuItem5 = new System.Windows.Forms.MenuItem();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.menuItem7 = new System.Windows.Forms.MenuItem();
			this.cmdPlay = new System.Windows.Forms.Button();
			this.txtFilename = new System.Windows.Forms.TextBox();
			this.cmdLoad = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem1,
																					  this.menuItem2});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem3,
																					  this.menuItem4,
																					  this.menuItem5,
																					  this.menuItem6});
			this.menuItem1.Text = "&File";
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 1;
			this.menuItem2.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem7});
			this.menuItem2.Text = "Help";
			// 
			// menuItem3
			// 
			this.menuItem3.Index = 0;
			this.menuItem3.Text = "&Open...";
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 1;
			this.menuItem4.Text = "Open &URL...";
			// 
			// menuItem5
			// 
			this.menuItem5.Index = 2;
			this.menuItem5.Text = "-";
			// 
			// menuItem6
			// 
			this.menuItem6.Index = 3;
			this.menuItem6.Text = "E&xit";
			// 
			// menuItem7
			// 
			this.menuItem7.Index = 0;
			this.menuItem7.Text = "&About";
			// 
			// cmdPlay
			// 
			this.cmdPlay.Location = new System.Drawing.Point(24, 80);
			this.cmdPlay.Name = "cmdPlay";
			this.cmdPlay.Size = new System.Drawing.Size(80, 24);
			this.cmdPlay.TabIndex = 0;
			this.cmdPlay.Text = "&Play";
			this.cmdPlay.Click += new System.EventHandler(this.cmdPlay_Click);
			// 
			// txtFilename
			// 
			this.txtFilename.Location = new System.Drawing.Point(24, 16);
			this.txtFilename.Name = "txtFilename";
			this.txtFilename.Size = new System.Drawing.Size(288, 20);
			this.txtFilename.TabIndex = 1;
			this.txtFilename.Text = "";
			// 
			// cmdLoad
			// 
			this.cmdLoad.Location = new System.Drawing.Point(128, 72);
			this.cmdLoad.Name = "cmdLoad";
			this.cmdLoad.Size = new System.Drawing.Size(104, 32);
			this.cmdLoad.TabIndex = 2;
			this.cmdLoad.Text = "&Load";
			this.cmdLoad.Click += new System.EventHandler(this.cmdLoad_Click);
			// 
			// frmDNPlay
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(472, 129);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.cmdLoad,
																		  this.txtFilename,
																		  this.cmdPlay});
			this.Menu = this.mainMenu1;
			this.Name = "frmDNPlay";
			this.Text = "DNPlay";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new frmDNPlay());
		}

		private void cmdPlay_Click(object sender, System.EventArgs e)
		{
			mPlayer.play();
		}

		private void cmdLoad_Click(object sender, System.EventArgs e)
		{
			String locFileName = txtFilename.Text;
			mPlayer.loadFile(locFileName);
		}
	}
}
