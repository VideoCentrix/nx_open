#pragma once

namespace nx::vms::client::desktop {

enum class AudioDecodeMode {
    /** Decode & play audio as usual. */
    normal,

    /** Decode & play audio & calculate audio spectrum. */
    normalWithSpectrum,

    /** Decode audio but don't forward it to a playback device, just prepare the audio spectrum.
     * This is used when displaying a spectrum on audio-only items that are muted. */
    spectrumOnly
};

} // namespace nx::vms::client::desktop
