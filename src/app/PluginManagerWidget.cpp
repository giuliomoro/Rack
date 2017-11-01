#include <thread>
#include "app.hpp"
#include "plugin.hpp"


namespace rack {


PluginManagerWidget::PluginManagerWidget() {
#ifndef RACK_NOGUI
	box.size.y = BND_WIDGET_HEIGHT;
	float margin = 5;
#endif /*RACK_NOGUI*/

#ifndef RACK_NOGUI
	{
		loginWidget = new Widget();
		Vec pos = Vec(0, 0);

		struct RegisterButton : Button {
			void onAction() override {
				std::thread t(openBrowser, "https://vcvrack.com/");
				t.detach();
			}
		};
		Button *registerButton = new RegisterButton();
		registerButton->box.pos = pos;
		registerButton->box.size.x = 75;
		registerButton->text = "Register";
		loginWidget->addChild(registerButton);
		pos.x += registerButton->box.size.x;

		pos.x += margin;
		TextField *emailField = new TextField();
		emailField->box.pos = pos;
		emailField->box.size.x = 175;
		emailField->placeholder = "Email";
		loginWidget->addChild(emailField);
		pos.x += emailField->box.size.x;

		pos.x += margin;
		PasswordField *passwordField = new PasswordField();
		passwordField->box.pos = pos;
		passwordField->box.size.x = 175;
		passwordField->placeholder = "Password";
		loginWidget->addChild(passwordField);
		pos.x += passwordField->box.size.x;

		struct LogInButton : Button {
			TextField *emailField;
			TextField *passwordField;
			void onAction() override {
				std::thread t(pluginLogIn, emailField->text, passwordField->text);
				t.detach();
				passwordField->text = "";
			}
		};
		pos.x += margin;
		LogInButton *logInButton = new LogInButton();
		logInButton->box.pos = pos;
		logInButton->box.size.x = 100;
		logInButton->text = "Log in";
		logInButton->emailField = emailField;
		logInButton->passwordField = passwordField;
		loginWidget->addChild(logInButton);
		pos.x += logInButton->box.size.x;

		struct StatusLabel : Label {
			void step() override {
				text = pluginGetLoginStatus();
			}
		};
		Label *label = new StatusLabel();
		label->box.pos = pos;
		loginWidget->addChild(label);

		addChild(loginWidget);
	}

	{
		manageWidget = new Widget();
		Vec pos = Vec(0, 0);

		struct ManageButton : Button {
			void onAction() override {
				std::thread t(openBrowser, "https://vcvrack.com/");
				t.detach();
			}
		};
		Button *manageButton = new ManageButton();
		manageButton->box.pos = pos;
		manageButton->box.size.x = 125;
		manageButton->text = "Manage plugins";
		manageWidget->addChild(manageButton);
		pos.x += manageButton->box.size.x;

		struct RefreshButton : Button {
			void onAction() override {
				std::thread t(pluginRefresh);
				t.detach();
			}
		};
		pos.x += margin;
		Button *refreshButton = new RefreshButton();
		refreshButton->box.pos = pos;
		refreshButton->box.size.x = 125;
		refreshButton->text = "Refresh plugins";
		manageWidget->addChild(refreshButton);
		pos.x += refreshButton->box.size.x;

		struct LogOutButton : Button {
			void onAction() override {
				pluginLogOut();
			}
		};
		pos.x += margin;
		Button *logOutButton = new LogOutButton();
		logOutButton->box.pos = pos;
		logOutButton->box.size.x = 100;
		logOutButton->text = "Log out";
		manageWidget->addChild(logOutButton);

		addChild(manageWidget);
	}

	{
		downloadWidget = new Widget();
		Vec pos = Vec(0, 0);

		struct DownloadProgressBar : ProgressBar {
			void step() override {
				label = "Downloading";
				std::string name = pluginGetDownloadName();
				if (name != "")
					label += " " + name;
				setValue(100.0 * pluginGetDownloadProgress());
			}
		};
		ProgressBar *downloadProgress = new DownloadProgressBar();
		downloadProgress->box.pos = pos;
		downloadProgress->box.size.x = 300;
		downloadProgress->setLimits(0, 100);
		downloadProgress->unit = "%";
		downloadWidget->addChild(downloadProgress);
		pos.x += downloadProgress->box.size.x;

		// struct CancelButton : Button {
		// 	void onAction() override {
		// 		pluginCancelDownload();
		// 	}
		// };
		// pos.x += margin;
		// Button *logOutButton = new CancelButton();
		// logOutButton->box.pos = pos;
		// logOutButton->box.size.x = 100;
		// logOutButton->text = "Cancel";
		// downloadWidget->addChild(logOutButton);

		addChild(downloadWidget);
	}
#endif /*RACK_NOGUI*/
}

void PluginManagerWidget::step() {
#ifndef RACK_NOGUI
	loginWidget->visible = false;
	manageWidget->visible = false;
	downloadWidget->visible = false;

	if (pluginIsDownloading())
		downloadWidget->visible = true;
	else if (pluginIsLoggedIn())
		manageWidget->visible = true;
	else
		loginWidget->visible = true;
#endif /*RACK_NOGUI*/

	Widget::step();
}


} // namespace rack
