#include "playermanager.h"
#include "videoplayer.h"
#include "jframe.h"

#include <iostream>

int main(int argc, char **argv)
{
	tuner::Locator locator("file://channels/channel3");

	media::VideoPlayer *player = (media::VideoPlayer *)media::PlayerManager::GetInstance()->CreatePlayer(&locator);

	if (player != NULL) {
		jgui::Frame window("", 0, 0, 1920, 1080);

		player->GetComponent()->SetBounds(0, 0, 1920, 1080);
		player->SetLoop(true);
		player->Play();
		
		window.Add(player->GetComponent());
		window.Show();
	}

	return 0;
}

