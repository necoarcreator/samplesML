from diffusers import DiffusionPipeline, DPMSolverMultistepScheduler
from moviepy import VideoFileClip, AudioFileClip
from diffusers import AudioLDMPipeline

from diffusers.utils import export_to_video
from scipy.io.wavfile import write
import os

import torch
import numpy as np
import time
from pprint import pprint

from gradio_client import Client


cwd = os.getcwd()
video_path = 'generated.mp4'
audio_path = 'audio.mp3'
final_name = "final_video.mp4"

num_frames = 40
fps = 10
audio_len = num_frames / fps
sample_rate = 16000
total_samples = int(audio_len * sample_rate)

# load video and audio pipeline

video_generator = DiffusionPipeline.from_pretrained("damo-vilab/text-to-video-ms-1.7b", \
                                                     torch_dtype=torch.float16, variant="fp16")
video_generator.scheduler = DPMSolverMultistepScheduler.from_config(video_generator.scheduler.config)
synthesiser =  AudioLDMPipeline.from_pretrained("cvssp/audioldm")
# optimize
video_generator.enable_model_cpu_offload()
video_generator.enable_vae_slicing()
synthesiser = synthesiser.to("cuda" if torch.cuda.is_available() else "cpu")

#write prompts
prompt_video = "A small retrowave-style palm is waving on an island. The red sun sets behind it."
prompt_audio = prompt_video + '. Generate an ambient sound for this'
# generate
video_frames = video_generator(prompt_video, num_inference_steps=25, num_frames=num_frames)
audio = synthesiser(prompt_audio, num_inference_steps=100, audio_length_in_s=audio_len).audios[0]
# add padding or delete unneccecary audio data
if len(audio) < total_samples:
    pad_len = total_samples - len(audio)
    audio = np.pad(audio, (0, pad_len), mode='constant')
else:
    audio = audio[:total_samples]
# convert to video
export_to_video(video_frames.frames[0], video_path, fps = fps);
write(audio_path, rate=sample_rate, data=audio);
#unite video and audio and make new file

try:
    with VideoFileClip(video_path) as video_clip:
        with AudioFileClip(audio_path) as audio_clip:

            final_clip = video_clip.with_audio(audio_clip)


            final_video_path = os.path.join(cwd, final_name)
            final_clip.write_videofile(final_video_path, codec="libx264", audio_codec="aac")

            print(f"Видео с аудио успешно сохранено: {final_video_path}")

except Exception as e:
    print("Ошибка при объединении видео и аудио:", e)

finally:
    time.sleep(1)
    if os.path.exists(video_path):
        os.remove(video_path)
    if os.path.exists(audio_path):
        os.remove(audio_path)

technical_query = 'Ты - опытный редактор текстов маркетингового агентства. \
                    Твоя задача - получать текст и переделывать его, чтобы он становился \
                    более "продающим" и качественным, чтобы он вызывал доверие у читателей. \
                    Исходный текст сгенерирован другой нейросетью. \
                    Пример: Открылся новый бар на ул. Каштановой -> О чудо! Наконец-то \
                    и на улице Каштановой можно отдохнуть! Вас ждут: напитки, тусовки \
                    и нереальный вайб лучших баров центра! \
                    Ещё пример: скидки в магазине одежды "забавные цены" -> готовь шубу летом \
                    - прошлый век! Подготовь свой стиль, а помогут тебе команда "забавных цен"! \
                    Только сейчас ликвидация моднейшей одежды! \
                    Запрос:'

text_to_transform = 'В этот увлекательный процесс можно погрузиться в Ресторане "ХХХ". \
    Здесь вас ждет не только разнообразное меню блюд, но и обширная коллекция виски, а \
        также возможность приобрести сертификат на дегустацию виски. Персонал ресторана \
            готов предложить наилучшие варианты для подчеркивания вашего вкусового опыта. \
                Вы можете ознакомиться с их ассортиментом на официальном сайте. \
                    Это погружение в мир ароматов и вкусов, которое оставит незабываемые \
                        воспоминания и создаст особенный опыт для ценителей качественного \
                            напитка и изысканной гастрономии.'
client = Client("hysts/mistral-7b", verbose=True);

try:
    result = client.predict(
                    message = technical_query + text_to_transform,
		            api_name="/chat",
                    param_3 = 0.4
                );
    client.reset_session();

except Exception as e:
    print(f"Ошибка при обращении к API: {str(e)}")

pprint(result)