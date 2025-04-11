import numpy as np
import matplotlib.pyplot as plt
import cv2
import os

from typing import Tuple

def negative_img(img: np.ndarray) -> np.ndarray:
    """
    Perform negative on the given img.
    :param img: An input grayscale image - ndarray of uint8 type.
    :return:
        neg_img: An output grayscale image after applying negative -
                      uint8 ndarray of size [H x W x 1]
    """
    neg_img = 255 - img
    return neg_img


def contrast_enhancement(img: np.ndarray) -> np.ndarray:
    """
    Perform contrast enhancement on the given img.
    :param img: An input grayscale image - ndarray of uint8 type.
    :return:
        contrast_enhanced_img: An output grayscale image after applying contrast enhancement -
                      uint8 ndarray of size [H x W x 1]
    """
    min_gs_val = np.min(img)
    max_gs_val = np.max(img)

    # handle division by zero edge-case
    if max_gs_val == min_gs_val:
        return img
    # otherwise perform contrast enhancement
    contrast_enhanced_img = ((img - min_gs_val) / (max_gs_val - min_gs_val) * 255).astype(np.uint8)
    return contrast_enhanced_img


def gamma_correction(img: np.ndarray, gamma: float) -> np.ndarray:
    """
    Perform gamma correction on a grayscale image.
    :param img: An input grayscale image - ndarray of uint8 type.
    :param gamma: the gamma parameter for the correction.
    :return:
        gamma_img: An output grayscale image after gamma correction -
                   uint8 ndarray of size [H x W x 1].
    """
    gamma_img = ((img / 255.0) ** gamma) * 255
    gamma_img = np.clip(gamma_img, 0, 255).astype(np.uint8)
    return gamma_img


def display_image_and_histogram(img: np.ndarray, title: str) -> None:
    """
    Display the given image and its histogram in separate figures.
    :param img: An input grayscale image - ndarray of uint8 type.
    :param title: Title for the image and histogram.
    """
    # figure with 1x2 grid of subplots -> [image], [histogram]
    fig, axs = plt.subplots(1, 2, figsize=(12, 6))

    # display the image
    axs[0].imshow(img, cmap='gray', vmin=0, vmax=255)
    axs[0].set_title(title)
    axs[0].axis('off')
    # display the histogram
    hist, bins = np.histogram(img.ravel(), bins=256, range=[0, 256])
    hist_normalized = hist / hist.sum()
    axs[1].bar(bins[:-1], hist_normalized, width=1, color='black', alpha=0.75)
    axs[1].set_title('Histogram for ' + title)
    axs[1].set_xlabel('Grayscale Value')
    axs[1].set_ylabel('Relative information_in_freq')

    plt.tight_layout()
    plt.show()


def handle_boat_image() -> None:
    boat_filepath = os.path.join('..', 'given_data', 'boat.jpg')
    boat_brightened_filepath = os.path.join('..', 'given_data', 'boat_brightened.jpg')
    # load the boat image and compute its variants (modifications)
    boat_img_gray = cv2.imread(boat_filepath, cv2.IMREAD_GRAYSCALE)
    boat_img_gray_neg = negative_img(boat_img_gray)
    boat_img_gray_contrast_enhanced = contrast_enhancement(boat_img_gray)
    # load the brightened boat image
    boat_img_brightened = cv2.imread(boat_brightened_filepath, cv2.IMREAD_GRAYSCALE)
    boat_img_brightened_contrast_enhanced = contrast_enhancement(boat_img_brightened)

    ################################
    # handle original boat image
    ################################
    display_image_and_histogram(boat_img_gray, 'Original Image')
    # display different modifications on the original image
    display_image_and_histogram(boat_img_gray_neg, 'Negative Image')
    display_image_and_histogram(boat_img_gray_contrast_enhanced, 'Contrast Enhanced Original Image')
    gamma_values = [0.3, 1.7]
    for g in gamma_values:
        display_image_and_histogram(gamma_correction(boat_img_gray, g), f'Gamma Corrected Image, (gamma = {g}')
    ################################
    # handle brightened image
    ################################
    display_image_and_histogram(boat_img_brightened, 'Boat Brightened Image')
    display_image_and_histogram(boat_img_brightened_contrast_enhanced, 'Contrast Enhanced Brightened Boat')
    ################################
    # filter the original boat image
    ################################
    kernel_3x3 = np.ones((3, 3), np.float32) / 9
    kernel_9x9 = np.ones((9, 9), np.float32) / 81
    boat_img_gray_avg_filtered_3x3 = cv2.filter2D(boat_img_gray, -1, kernel_3x3)
    display_image_and_histogram(boat_img_gray_avg_filtered_3x3, 'Boat Image Average Filtered 3x3')
    boat_img_gray_avg_filtered_9x9 = cv2.filter2D(boat_img_gray, -1, kernel_9x9)
    display_image_and_histogram(boat_img_gray_avg_filtered_9x9, 'Boat Image Average Filtered 9x9')
    boat_img_gray_median_filtered_9x9 = cv2.medianBlur(boat_img_gray, 9)
    display_image_and_histogram(boat_img_gray_median_filtered_9x9, 'Boat Image Median Filtered 9x9')


def plot_in_subplot(fig, image: np.ndarray, index: Tuple[int, int, int],
                    title: str = None, cmap: str = None, show_axis: bool = True,
                    x_label: str = None, y_label: str = None) -> None:
    plt.figure(fig.number)
    plt.subplot(*index)
    plt.title(title)
    plt.imshow(image, cmap=cmap)
    if not show_axis:
        plt.axis('off')
    if x_label:
        plt.xlabel(x_label)
    if y_label:
        plt.ylabel(y_label)


def low_pass_filter_x(shape: Tuple[int, int], cutoff_ratio: float) -> np.ndarray:
    rows, cols = shape
    mask = np.zeros((rows, cols), np.uint8)
    cutoff = int(cols * cutoff_ratio)
    # create a centered mask
    mask[:, cols//2 - cutoff:cols//2 + cutoff] = 1
    return mask


def low_pass_filter_y(shape: Tuple[int, int], cutoff_ratio: float) -> np.ndarray:
    rows, cols = shape
    mask = np.zeros((rows, cols), np.uint8)
    cutoff = int(rows * cutoff_ratio)
    # create a centered mask
    mask[rows//2 - cutoff:rows//2 + cutoff, :] = 1
    return mask


def gaussian(N: int, sigma: float) -> np.ndarray:
    x = np.arange(N) - N//2
    g_curve = np.exp(-0.5 * (x / sigma)**2)
    return g_curve


def max_freq_filtering(fshift: np.ndarray, percentage: float) -> Tuple[np.ndarray, np.ndarray]:
    """
    Reconstruct an image using only its maximal amplitude frequencies.
    :param fshift: The fft of an image, **after fftshift** - complex float ndarray of size [H x W].
    :param percentage: the wanted percentage of frequencies of highest amplitude.
    :return:
        fMaxFreq: The filtered information_in_freq domain result - complex float ndarray of size [H x W].
        imgMaxFreq: The filtered image - real float ndarray of size [H x W].
    """

    magnitude_spectrum = np.abs(fshift)
    # sort the magnitudes in descending order and get the corresponding indices
    sorted_indices = np.argsort(-magnitude_spectrum, axis=None)

    num_frequencies_to_retain = int(np.ceil((percentage / 100) * magnitude_spectrum.size))
    mask = np.zeros_like(magnitude_spectrum, dtype=bool)
    mask.flat[sorted_indices[:num_frequencies_to_retain]] = True
    fMaxFreq = fshift * mask

    # reconstruct the filtered image
    imgMaxFreq = np.fft.ifft2(np.fft.ifftshift(fMaxFreq))
    imgMaxFreq = np.real(imgMaxFreq)

    return fMaxFreq, imgMaxFreq


def display_mff_results(fMaxFreq: np.ndarray, imgMaxFreq: np.ndarray, percentage: float) -> None:
    """
    Display the results of the Max Frequency Filtering of the image, obtained by max_freq_filtering.
    :param fMaxFreq: The filtered information_in_freq domain result - complex float ndarray of size [H x W].
    :param imgMaxFreq: The filtered image - real float ndarray of size [H x W].
    :param percentage: Percentage of retained maximal amplitude frequencies.
    :return: None
    """

    fig, axs = plt.subplots(1, 2, figsize=(16, 8))
    plot_in_subplot(fig, np.log(1 + np.abs(fMaxFreq)), (1, 2, 1),
                    title=f'Filtered Maximum Frequencies ({percentage}%)', cmap='gray', show_axis=False)
    plot_in_subplot(fig, abs(imgMaxFreq), (1, 2, 2),
                    title='Maximum Frequencies Filtered Image', cmap='gray', show_axis=False)
    plt.tight_layout()


def mse_impact(original_img: np.ndarray) -> None:
    """
    Calculate and plot the Mean Squared Error (MSE) impact as a
    function of the percentage of maximum frequencies filtered.
    :param original_img: The original image as a 2D NumPy array.
    :return: None
    """
    M, N = original_img.shape
    mse_values = []
    p_values = list(range(int(1), int(101)))
    for p in p_values:
        fshift = np.fft.fftshift(np.fft.fft2(original_img))
        dummy, max_filtered_image = max_freq_filtering(fshift, p)
        mse_p = np.sum((original_img - max_filtered_image)**2) / (M * N)
        mse_values.append(mse_p)

    plt.figure()
    plt.plot(p_values, mse_values)
    plt.xlabel('p (percentage of maximum frequencies filtered)')
    plt.ylabel('Mean Squared Error (MSE)')
    plt.title('MSE Impact as a Function of p')
    plt.grid(True)


def information_in_freq() -> None:
    cafeteria_filepath = os.path.join('..', 'my_data', 'cafeteria.jpg')
    cafeteria_img = cv2.cvtColor(cv2.imread(cafeteria_filepath), cv2.COLOR_BGR2RGB)
    cafeteria_img_gray = cv2.imread(cafeteria_filepath, cv2.IMREAD_GRAYSCALE).astype(np.uint8)
    ##############################
    # 2-a
    cafeteria_img_fig = plt.figure(figsize=(16, 8))
    plot_in_subplot(cafeteria_img_fig, cafeteria_img, (1, 2, 1), 'Original Image', show_axis=False)
    plot_in_subplot(cafeteria_img_fig, cafeteria_img_gray, (1, 2, 2), 'Grayscale Image',
                    cmap='gray', show_axis=False)
    plt.tight_layout()
    ##############################
    # 2-b 2D-DFT
    cafeteria_img_dft = np.fft.fft2(cafeteria_img_gray)
    cafeteria_img_dft_shifted = np.fft.fftshift(cafeteria_img_dft)
    amplitude_spectrum = np.log(1 + np.abs(cafeteria_img_dft_shifted))
    plot_in_subplot(plt.figure(figsize=(12, 8)), amplitude_spectrum, (1, 1, 1), '2D-DFT Amplitude',
                    cmap='gray', x_label='Frequency X', y_label='Frequency Y')
    ##############################
    # 2-c LPF
    cutoff_ratio = 0.02
    mask_x = low_pass_filter_x(cafeteria_img_gray.shape, cutoff_ratio)
    mask_y = low_pass_filter_y(cafeteria_img_gray.shape, cutoff_ratio)
    mask_combined = mask_x + mask_y
    # apply the masks to the 2D-DFT coefficients
    dft_shift_x_filtered = cafeteria_img_dft_shifted * mask_x
    dft_shift_y_filtered = cafeteria_img_dft_shifted * mask_y
    dft_shift_combined_filtered = cafeteria_img_dft_shifted * mask_combined
    # create 1-D Gaussian
    gaussian_function = gaussian(cafeteria_img.shape[1], 10)
    gaussian_matrix = np.tile(gaussian_function, (cafeteria_img.shape[0], 1))
    # multiply the masks with the gaussian in a point-wise manner
    freq_image_x = gaussian_matrix * dft_shift_x_filtered
    freq_image_y = gaussian_matrix * dft_shift_y_filtered
    freq_image_combined = gaussian_matrix * dft_shift_combined_filtered
    # perform 2D-IDFT and shift
    img_restored_x = np.fft.ifft2(np.fft.ifftshift(freq_image_x))
    img_restored_y = np.fft.ifft2(np.fft.ifftshift(freq_image_y))
    img_restored_combined = np.fft.ifft2(np.fft.ifftshift(freq_image_combined))
    # display the results
    plot_in_subplot(plt.figure(figsize=(12, 8)), gaussian_matrix, (1, 1, 1),
                    title='Gaussian Matrix as an Image', cmap='gray', show_axis=False)
    fig = plt.figure(figsize=(20, 10))
    plot_in_subplot(fig, np.log(1 + np.abs(freq_image_x)), (2, 3, 1),
                    title='LPF-ed Frequency Image 2% (X-axis)', cmap='gray')
    plot_in_subplot(fig, np.log(1 + np.abs(freq_image_y)), (2, 3, 2),
                    title='LPF-ed Frequency Image 2% (Y-axis)', cmap='gray')
    plot_in_subplot(fig, np.log(1 + np.abs(freq_image_combined)), (2, 3, 3),
                    title='LPF-ed Frequency Image 2% (X and Y-axis)', cmap='gray')
    plot_in_subplot(fig, np.abs(img_restored_x), (2, 3, 4),
                    title='Restored LPF-ed Image (X-axis)', cmap='gray')
    plot_in_subplot(fig, np.abs(img_restored_y), (2, 3, 5),
                    title='Restored LPF-ed Image (Y-axis)', cmap='gray')
    plot_in_subplot(fig, np.abs(img_restored_combined), (2, 3, 6),
                    title='Restored LPF-ed Image (X and Y-axis)', cmap='gray')

    plt.tight_layout()
    ##############################
    # 2-d MPF
    fMaxFreq_10, imgMaxFreq_10 = max_freq_filtering(cafeteria_img_dft_shifted, 10)
    display_mff_results(fMaxFreq_10, imgMaxFreq_10, 10)
    fMaxFreq_4, imgMaxFreq_4 = max_freq_filtering(cafeteria_img_dft_shifted, 4)
    display_mff_results(fMaxFreq_4, imgMaxFreq_4, 4)
    ##############################
    # 2-e MSE(p)
    mse_impact(cafeteria_img_gray)
    ##############################
    plt.show()


def phase_and_amplitude() -> None:
    parrot_filepath = os.path.join('..', 'given_data', 'parrot.png')
    selfie_filepath = os.path.join('..', 'my_data', 'yours.jpg')
    ##############################
    # 3-a - handle the input
    parrot_img_gray = cv2.imread(parrot_filepath, cv2.IMREAD_GRAYSCALE).astype(np.uint8)
    selfie_img_gray = cv2.resize(cv2.imread(selfie_filepath, cv2.IMREAD_GRAYSCALE).astype(np.uint8),
                                 parrot_img_gray.shape)
    fig, axs = plt.subplots(1, 2, figsize=(20, 10))
    plot_in_subplot(fig, parrot_img_gray, (1, 2, 1), 'Parrot Image', cmap='gray', show_axis=False)
    plot_in_subplot(fig, selfie_img_gray, (1, 2, 2), 'Selfie Image', cmap='gray', show_axis=False)

    ##############################
    # 3-b - calculate 2D-DFT - amplitude and phase
    parrot_dft = np.fft.fftshift(np.fft.fft2(parrot_img_gray))
    parrot_dft_amp = np.abs(parrot_dft)
    parrot_dft_phase = np.angle(parrot_dft)
    selfie_dft = np.fft.fftshift(np.fft.fft2(selfie_img_gray))
    selfie_dft_amp = np.abs(selfie_dft)
    selfie_dft_phase = np.angle(selfie_dft)
    fig, axs = plt.subplots(1, 2, figsize=(20, 10))
    plot_in_subplot(fig, np.log(1 + parrot_dft_amp), (1, 2, 1), 'Parrot Image 2D-DFT Amplitude', cmap='gray')
    plot_in_subplot(fig, np.log(1 + selfie_dft_amp), (1, 2, 2), 'Selfie Image 2D-DFT Amplitude', cmap='gray')

    ##############################
    # 3-c mix things up
    # image with selfie amplitude and parrot phase
    transformed_img1 = selfie_dft_amp * np.exp(1j * parrot_dft_phase)
    # image with parrot amplitude and selfie phase
    transformed_img2 = parrot_dft_amp * np.exp(1j * selfie_dft_phase)

    restored_img1 = np.fft.ifft2(np.fft.ifftshift(transformed_img1))
    restored_img2 = np.fft.ifft2(np.fft.ifftshift(transformed_img2))

    fig, axs = plt.subplots(1, 2, figsize=(20, 10))
    plot_in_subplot(fig, abs(restored_img1), (1, 2, 1),
                    title='Selfie Amplitude, Parrot Phase', cmap='gray', show_axis=False)
    plot_in_subplot(fig, abs(restored_img2), (1, 2, 2),
                    title='Parrot Amplitude, Selfie Phase', cmap='gray', show_axis=False)

    ##############################
    # 3-d let's be random
    rows, cols = selfie_img_gray.shape
    random_amp = np.random.uniform(low=0, high=255, size=(rows, cols))
    random_phase = np.random.uniform(low=-np.pi, high=np.pi, size=(rows, cols))

    random_img1 = random_amp * np.exp(1j * selfie_dft_phase)
    random_img2 = selfie_dft_amp * np.exp(1j * random_phase)
    fig, axs = plt.subplots(1, 2, figsize=(20, 10))
    plot_in_subplot(fig, abs(np.fft.ifft2(np.fft.ifftshift(random_img2))), (1, 2, 1),
                    title='Random Amplitude, Selfie Phase', cmap='gray', show_axis=False)
    plot_in_subplot(fig, abs(np.fft.ifft2(np.fft.ifftshift(random_img1))), (1, 2, 2),
                    title='Selfie Amplitude, Random Phase', cmap='gray', show_axis=False)
    ##############################
    plt.show()


if __name__ == '__main__':
    handle_boat_image()
    information_in_freq()
    phase_and_amplitude()
    pass
