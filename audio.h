#ifndef audio_h
#define audio_h

#include "public.h"

class AudioSystem
{
    public:
        bool muted;
        map<string, Mix_Music*> songs;
        map<string, Mix_Chunk*> sounds;

        Mix_Music* LoadSong(string path);
        Mix_Chunk* LoadSound(string path);
        AudioSystem();
        void Init();
        void Mute();
        void LoadContent();
        void PlaySong(string song);
        void PlaySound(string sound);
        void Update();
};
AudioSystem::AudioSystem(void)
{

}
void AudioSystem::Init()
{
    //Mix_Init(MIX_INIT_OGG);
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
        printf("Audio system failed to initialize");

    muted = g("audio-start-muted");
}

Mix_Music* AudioSystem::LoadSong(string path)
{
    if(path=="")
        return NULL;

    cout << "Loading " << path << "...";
    Mix_Music* mus = Mix_LoadMUS(string("data/music/"+path).c_str());
    cout << "Done!\n";
    return mus;
}
Mix_Chunk* AudioSystem::LoadSound(string path)
{
    if(path=="")
        return NULL;

    cout << "Loading " << path << "...";
    Mix_Chunk* sound = Mix_LoadWAV(string("data/music/"+path).c_str());
    cout << "Done!\n";
    return sound;
}

void AudioSystem::LoadContent()
{
    ifstream in("data/music/songs.txt");
    ifstream in2("data/music/sounds.txt");

    string line;
    while(!in.eof())
    {
        std::getline(in,line);
        //cout << FirstWord(line) << endl;
        songs[FirstWord(line)]=LoadSong(SecondWord(line));
        audioFiles++;
    };
    while(!in2.eof())
    {
        std::getline(in2,line);
        sounds[FirstWord(line)]=LoadSound(SecondWord(line));
        audioFiles++;
    }
    PlaySong("cat");

}


void AudioSystem::PlaySong(string song)
{
    if(songs.count(song))
        Mix_PlayMusic(songs[song], -1);
    //cout << "played";
}

void AudioSystem::PlaySound(string sound)
{
    cout << "playing sound" << endl;
    if(sounds.count(sound))
        Mix_PlayChannel(-1,sounds[sound], 0);
}
void AudioSystem::Mute()
{
    muted = !muted;
    if(muted)
    {
        Mix_VolumeMusic(0);
    }
    else
    {
        Mix_VolumeMusic(128);
    }
}
void AudioSystem::Update()
{
    while(audioCommands.size()>0)
    {
        string c = audioCommands.front();
        audioCommands.pop();
        if(c == "mute")
            Mute();
        else if(FirstWord(c)=="sound")
            PlaySound(SecondWord(c));
        else if(FirstWord(c)=="music")
            PlaySong(SecondWord(c));
    }
}
#endif
