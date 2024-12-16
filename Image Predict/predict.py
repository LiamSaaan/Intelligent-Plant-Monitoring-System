from googleapiclient.discovery import build
from googleapiclient.http import MediaIoBaseDownload
from google.oauth2.credentials import Credentials
from PIL import Image
import tensorflow as tf
import numpy as np
import os
import io

# Import the utility functions
from utils import clean_image, get_prediction, make_results

# Authenticate and initialize the Drive API
def authenticate_google_drive():
    creds = None
    if os.path.exists('token.json'):
        creds = Credentials.from_authorized_user_file('token.json', ['https://www.googleapis.com/auth/drive'])
    else:
        from google_auth_oauthlib.flow import InstalledAppFlow
        flow = InstalledAppFlow.from_client_secrets_file(
            'credentials.json', ['https://www.googleapis.com/auth/drive'])
        creds = flow.run_local_server(port=0)
        with open('token.json', 'w') as token:
            token.write(creds.to_json())
    return build('drive', 'v3', credentials=creds)

# Get the latest file from a specified folder
def get_latest_file(service, folder_name):
    # Search for the folder by name
    query = f"name='{folder_name}' and mimeType='application/vnd.google-apps.folder'"
    response = service.files().list(q=query, spaces='drive').execute()
    folder = response.get('files', [])
    if not folder:
        print(f"Folder '{folder_name}' not found.")
        return None

    folder_id = folder[0]['id']

    # Get all files in the folder, sorted by creation date
    query = f"'{folder_id}' in parents and mimeType contains 'image/'"
    response = service.files().list(q=query, spaces='drive',
                                     orderBy='createdTime desc',
                                     fields='files(id, name)').execute()
    files = response.get('files', [])
    if not files:
        print("No files found in the folder.")
        return None
    return files[0]  # Return the latest file

# Download a file from Google Drive
def download_file(service, file_id, file_name):
    request = service.files().get_media(fileId=file_id)
    file_path = os.path.join(os.getcwd(), file_name)
    with io.FileIO(file_path, 'wb') as fh:
        downloader = MediaIoBaseDownload(fh, request)
        done = False
        while not done:
            status, done = downloader.next_chunk()
            print(f"Download progress: {int(status.progress() * 100)}%")
    print(f"File '{file_name}' downloaded to {file_path}")
    return file_path

# Load the prediction model
def load_model(path):
    xception_model = tf.keras.models.Sequential([
        tf.keras.applications.xception.Xception(include_top=False, weights='imagenet', input_shape=(512, 512, 3)),
        tf.keras.layers.GlobalAveragePooling2D(),
        tf.keras.layers.Dense(4, activation='softmax')
    ])

    densenet_model = tf.keras.models.Sequential([
        tf.keras.applications.densenet.DenseNet121(include_top=False, weights='imagenet', input_shape=(512, 512, 3)),
        tf.keras.layers.GlobalAveragePooling2D(),
        tf.keras.layers.Dense(4, activation='softmax')
    ])

    inputs = tf.keras.Input(shape=(512, 512, 3))
    xception_output = xception_model(inputs)
    densenet_output = densenet_model(inputs)
    outputs = tf.keras.layers.average([densenet_output, xception_output])

    model = tf.keras.Model(inputs=inputs, outputs=outputs)
    model.load_weights(path)
    return model

# Main function to process the image and make predictions
def process_and_predict_image(image_path, model):
    # Open the image
    image = Image.open(image_path)

    # Clean the image
    processed_image = clean_image(image)

    # Get predictions
    predictions, predictions_arr = get_prediction(model, processed_image)

    # Make results
    result = make_results(predictions, predictions_arr)
    return result

if __name__ == '__main__':
    # Authenticate with Google Drive
    service = authenticate_google_drive()

    # Specify folder name
    folder_name = 'ESP32-CAM'

    # Get the latest file
    latest_file = get_latest_file(service, folder_name)
    if latest_file:
        print(f"Downloading latest file: {latest_file['name']}")
        local_file_path = download_file(service, latest_file['id'], latest_file['name'])

        # Load the plant disease detection model
        model = load_model('model.h5')

        # Predict the condition of the plant
        print("Analyzing the plant image...")
        result = process_and_predict_image(local_file_path, model)

        # Display the result
        print(f"The plant {result['status']} with {result['prediction']} prediction.")
