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
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Label lblFileLocation;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.OpenFileDialog dlgOpenFile;
		private System.Windows.Forms.Button cmdStop;
		private System.Windows.Forms.Button cmdPause;

		//My Variables...
		protected DSPlay mPlayer;
		private System.Windows.Forms.Label lblDuration;
		protected Int64 mFileDuration;
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
			this.lblFileLocation = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.dlgOpenFile = new System.Windows.Forms.OpenFileDialog();
			this.cmdStop = new System.Windows.Forms.Button();
			this.cmdPause = new System.Windows.Forms.Button();
			this.lblDuration = new System.Windows.Forms.Label();
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
			this.menuItem3.Click += new System.EventHandler(this.menuItem3_Click);
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
			// lblFileLocation
			// 
			this.lblFileLocation.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblFileLocation.Location = new System.Drawing.Point(96, 8);
			this.lblFileLocation.Name = "lblFileLocation";
			this.lblFileLocation.Size = new System.Drawing.Size(368, 16);
			this.lblFileLocation.TabIndex = 3;
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(8, 8);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(80, 16);
			this.label2.TabIndex = 4;
			this.label2.Text = "File Location";
			// 
			// dlgOpenFile
			// 
			this.dlgOpenFile.Title = "Select a file to play...";
			// 
			// cmdStop
			// 
			this.cmdStop.Location = new System.Drawing.Point(120, 80);
			this.cmdStop.Name = "cmdStop";
			this.cmdStop.Size = new System.Drawing.Size(72, 24);
			this.cmdStop.TabIndex = 5;
			this.cmdStop.Text = "&Stop";
			this.cmdStop.Click += new System.EventHandler(this.cmdStop_Click);
			// 
			// cmdPause
			// 
			this.cmdPause.Location = new System.Drawing.Point(208, 80);
			this.cmdPause.Name = "cmdPause";
			this.cmdPause.Size = new System.Drawing.Size(72, 24);
			this.cmdPause.TabIndex = 6;
			this.cmdPause.Text = "Pa&use";
			this.cmdPause.Click += new System.EventHandler(this.cmdPause_Click);
			// 
			// lblDuration
			// 
			this.lblDuration.Location = new System.Drawing.Point(344, 32);
			this.lblDuration.Name = "lblDuration";
			this.lblDuration.Size = new System.Drawing.Size(112, 24);
			this.lblDuration.TabIndex = 7;
			// 
			// frmDNPlay
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(472, 129);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.lblDuration,
																		  this.cmdPause,
																		  this.cmdStop,
																		  this.label2,
																		  this.lblFileLocation,
																		  this.cmdPlay});
			this.Menu = this.mainMenu1;
			this.Name = "frmDNPlay";
			this.Text = "DNPlay";
			this.Load += new System.EventHandler(this.frmDNPlay_Load);
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

		
		private void menuItem3_Click(object sender, System.EventArgs e)
		{
			//File->Open
			dlgOpenFile.CheckFileExists = true;
			DialogResult locResult = dlgOpenFile.ShowDialog();
			if (locResult == DialogResult.OK) 
			{
				lblFileLocation.Text = dlgOpenFile.FileName;
				bool locRes = mPlayer.loadFile(dlgOpenFile.FileName);
				//Error check
				mFileDuration = mPlayer.fileDuration();
				lblDuration.Text = mFileDuration.ToString();
			}
			
		}

		private void cmdStop_Click(object sender, System.EventArgs e)
		{
			mPlayer.stop();
		}

		private void cmdPause_Click(object sender, System.EventArgs e)
		{
			mPlayer.pause();
		}

		private void frmDNPlay_Load(object sender, System.EventArgs e)
		{
		
		}
	}
}
