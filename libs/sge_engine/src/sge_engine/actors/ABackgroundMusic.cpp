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
		ReflMember(ABackgroundMusic, m_volume).uiRange(0.0f, 1.0f, 0.01f)
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
		(*audioAsset)->trackVolume = m_volume;
		getCore()->getAudioDevice()->setBackgroundMusic(audioAsset->get());
	} else {
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
	const bool assetChanged = m_backgroundMusic.update();
	auto audioAsset = m_backgroundMusic.getAssetAudio();
	if (assetChanged && audioAsset) {
		(*audioAsset)->decoder.seekToBegining();
	}

	if (m_wasPlaying && updateSets.isGamePaused()) {
		setBackgroundMusicInAudioDevice(false);
	} else if (!m_wasPlaying && updateSets.isPlaying()) {
		setBackgroundMusicInAudioDevice(true);
	} else if (updateSets.isPlaying() && assetChanged) {
		setBackgroundMusicInAudioDevice(true);
	} else if (audioAsset && (*audioAsset)->trackVolume != m_volume) {
		setBackgroundMusicInAudioDevice(true);
	}
	m_wasPlaying = updateSets.isPlaying();
}

void ABackgroundMusic::onPlayStateChanged(bool const isStartingToPlay) {
	Actor::onPlayStateChanged(isStartingToPlay);
	if (!isStartingToPlay) {
		setBackgroundMusicInAudioDevice(false);
	}

	auto audioAsset = m_backgroundMusic.getAssetAudio();
	if (audioAsset) {
		(*audioAsset)->decoder.seekToBegining();
	}
}

} // namespace sge