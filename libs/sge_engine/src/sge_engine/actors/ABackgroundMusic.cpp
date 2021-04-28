#include "sge_core/ICore.h"
#include "sge_engine/GameWorld.h"
#include "ABackgroundMusic.h"

namespace sge {

//--------------------------------------------------------------------
// ABackgroundMusic
//--------------------------------------------------------------------
DefineTypeId(ABackgroundMusic, 21'04'28'0001);
ReflBlock() {
	ReflAddActor(ABackgroundMusic)
		ReflMember(ABackgroundMusic, m_backgroundMusic)
	;
}

AABox3f ABackgroundMusic::getBBoxOS() const
{
	return AABox3f();
}

void ABackgroundMusic::setBackgroundMusicInAudioDevice(bool playing) {
	auto audioAsset = m_backgroundMusic.getAssetAudio();
	if (!audioAsset) {
		return;
	}
	if (playing) {
		getCore()->getAudioDevice()->setBackgroundMusic(audioAsset->get());
	} else {
		(*audioAsset)->decoder.seekToBegining();
		getCore()->getAudioDevice()->setBackgroundMusic(nullptr);
	}
}

ABackgroundMusic::ABackgroundMusic()
    : m_backgroundMusic(AssetType::Audio) {
}

void ABackgroundMusic::create() {

}

void ABackgroundMusic::update(const GameUpdateSets &updateSets) {
	Actor::update(updateSets);
	m_backgroundMusic.update();

     if (m_wasPlaying && updateSets.isGamePaused()) {
		setBackgroundMusicInAudioDevice(false);
     }
     if (!m_wasPlaying && updateSets.isPlaying()) {
	     setBackgroundMusicInAudioDevice(true);
     }
	 m_wasPlaying = updateSets.isPlaying();
}

} // namespace sge