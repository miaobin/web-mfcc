const audioContext = new AudioContext();
const audioElement = document.getElementById("audio");

let runtime = false;

Module.onRuntimeInitialized = function() {
  runtime = true;
  console.log('WASM Runtime Ready.');
};

async function startAnalyse() {
  if(audioContext.state != "running") {
    audioContext.resume().then(function() {
      console.log('audioContext resumed.')
    });
  }
  audioElement.play();

  if(runtime) {
    let pcm = await getAudioPCMData(audioElement);
    // windowSize = sampleRate * windowSize_ms / 1000
    // windowStride = sampleRate * windowStride_ms / 1000
    let mfccs = getAudioMfccs(pcm, audioContext.sampleRate, 1323, 882);
    console.log("mfccs value:", mfccs);
  } else {
    console.log('WASM Runtime ERROR!');
  }
}

async function getAudioPCMData(audio) {
  let request = new Request(audio.src);
  let response = await fetch(request);
  let audioFileData = await response.arrayBuffer();
  let audioDecodeData = await audioContext.decodeAudioData(audioFileData);
  let audioPCMData = audioDecodeData.getChannelData(0);

  return audioPCMData;
}

function getAudioMfccs(pcm, sampleRate,
                       windowSize, windowStride,
                       upperFrequencyLimit = 4000,
                       lowerFrequencyLimit = 20,
                       filterbankChannelCount = 40,
                       dctCoefficientCount = 13) {
  let pcmPtr = Module._malloc(8 * pcm.length);
  let lenPtr = Module._malloc(4);

  for(let i=0; i<pcm.length; i++) {
    Module.HEAPF64[pcmPtr/8 + i] = pcm[i];
  }
  Module.HEAP32[lenPtr/4] = pcm.length;

  let tfMfccs = Module.cwrap('tf_mfccs', 'number',
                            ['number', 'number', 'number', 'number',
                             'number', 'number', 'number', 'number', 'number']);
  let mfccsPtr = tfMfccs(pcmPtr, lenPtr,
                         sampleRate, windowSize, windowStride,
                         upperFrequencyLimit, lowerFrequencyLimit,
                         filterbankChannelCount, dctCoefficientCount);
  let mfccsLen = Module.HEAP32[lenPtr >> 2];
  let audioMfccs = [mfccsLen];

  for(let i=0; i<mfccsLen; i++) {
    audioMfccs[i] = Module.HEAPF64[(mfccsPtr >> 3) + i];
  }

  Module._free(pcmPtr, lenPtr, mfccsPtr);

  return audioMfccs;
}
