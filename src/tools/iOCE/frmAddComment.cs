using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace iOCE
{
	/// <summary>
	/// Summary description for frmAddComment.
	/// </summary>
	public class frmAddComment : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.ComboBox cboKey;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox txtValue;
		private System.Windows.Forms.Button cmdOK;
		private System.Windows.Forms.Button cmdCancel;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public String Key;
		public String Value;
		public bool wasOK;

		public frmAddComment()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			wasOK = false;
			Key = "";
			Value = "";
			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
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
			this.label1 = new System.Windows.Forms.Label();
			this.cboKey = new System.Windows.Forms.ComboBox();
			this.label2 = new System.Windows.Forms.Label();
			this.txtValue = new System.Windows.Forms.TextBox();
			this.cmdOK = new System.Windows.Forms.Button();
			this.cmdCancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label1.Location = new System.Drawing.Point(16, 16);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(48, 24);
			this.label1.TabIndex = 0;
			this.label1.Text = "Key";
			// 
			// cboKey
			// 
			this.cboKey.Items.AddRange(new object[] {
														"TITLE",
														"ARTIST",
														"ALBUM",
														"GENRE",
														"TRACKNUMBER",
														"DESCRIPTION",
														"COPYRIGHT",
														"DATE",
														"PERFORMER",
														"VERSION",
														"ORGANIZATION",
														"LOCATION",
														"CONTACT",
														"ISRC"});
			this.cboKey.Location = new System.Drawing.Point(80, 16);
			this.cboKey.Name = "cboKey";
			this.cboKey.Size = new System.Drawing.Size(272, 21);
			this.cboKey.TabIndex = 1;
			// 
			// label2
			// 
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label2.Location = new System.Drawing.Point(16, 48);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(48, 24);
			this.label2.TabIndex = 2;
			this.label2.Text = "Value";
			// 
			// txtValue
			// 
			this.txtValue.Location = new System.Drawing.Point(80, 48);
			this.txtValue.Name = "txtValue";
			this.txtValue.Size = new System.Drawing.Size(272, 20);
			this.txtValue.TabIndex = 3;
			this.txtValue.Text = "";
			// 
			// cmdOK
			// 
			this.cmdOK.Location = new System.Drawing.Point(200, 80);
			this.cmdOK.Name = "cmdOK";
			this.cmdOK.Size = new System.Drawing.Size(72, 24);
			this.cmdOK.TabIndex = 4;
			this.cmdOK.Text = "&OK";
			this.cmdOK.Click += new System.EventHandler(this.cmdOK_Click);
			// 
			// cmdCancel
			// 
			this.cmdCancel.Location = new System.Drawing.Point(280, 80);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.Size = new System.Drawing.Size(72, 24);
			this.cmdCancel.TabIndex = 5;
			this.cmdCancel.Text = "&Cancel";
			this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
			// 
			// frmAddComment
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(368, 118);
			this.Controls.Add(this.cmdCancel);
			this.Controls.Add(this.cmdOK);
			this.Controls.Add(this.txtValue);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.cboKey);
			this.Controls.Add(this.label1);
			this.Name = "frmAddComment";
			this.Text = "Add Comment...";
			this.Load += new System.EventHandler(this.frmAddComment_Load);
			this.ResumeLayout(false);

		}
		#endregion

		private void frmAddComment_Load(object sender, System.EventArgs e)
		{
			wasOK = false;
			cboKey.Text = Key;
			txtValue.Text = Value;
		}

		private void cmdCancel_Click(object sender, System.EventArgs e)
		{
			wasOK = false;
			Key = "";
			Value = "";
			this.Close();
		}

		private void cmdOK_Click(object sender, System.EventArgs e)
		{
			wasOK = true;
			Key = cboKey.Text;
			Value = txtValue.Text;
			this.Close();
		}
	}
}
