#pragma once

#include "sge_engine/Actor.h"

#include <sge_engine/AssetProperty.h>

namespace sge {
//--------------------------------------------------------------------
// ABackgroundMusic
//--------------------------------------------------------------------
struct SGE_ENGINE_API ABackgroundMusic : public Actor
{
	ABackgroundMusic();
	void create() final;
	//void onPlayStateChanged(bool const isStartingToPlay) final;
	void update(const GameUpdateSets& updateSets) final;
	AABox3f getBBoxOS() const final;

	AssetProperty m_backgroundMusic;

  private:
	void setBackgroundMusicInAudioDevice(bool playing);
	bool m_wasPlaying = false;
};


}
