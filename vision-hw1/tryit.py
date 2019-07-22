from uwimg import *

# NN - upsize
im = load_image("data/dogsmall.jpg")
a = nn_resize(im, im.w*4, im.h*4)
save_image(a, "dog4x-nn")

# Bilinear - upsize
im = load_image("data/dogsmall.jpg")
a = bilinear_resize(im, im.w*4, im.h*4)
save_image(a, "dog4x-bl")

# NN - downsize
im = load_image("data/dog.jpg")
a = nn_resize(im, im.w//7, im.h//7)
save_image(a, "dog7th-nn")

# Bilinear - downsize
im = load_image("data/dog.jpg")
a = bilinear_resize(im, im.w//7, im.h//7)
save_image(a, "dog7th-bl")

# Box filter
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-box7")

# Box filter + Bilinear downsize
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "dogthumb")

# High pass filter
im = load_image("data/dog.jpg")
f = make_highpass_filter()
blur = convolve_image(im, f, 0)
clamp_image(blur)
save_image(blur, "dog_high_pass")

# Sharpen filter
im = load_image("data/dog.jpg")
f = make_sharpen_filter()
blur = convolve_image(im, f, 1)
clamp_image(blur)
save_image(blur, "dog_sharpen")

# Emboss filter
im = load_image("data/dog.jpg")
f = make_emboss_filter()
blur = convolve_image(im, f, 1)
clamp_image(blur)
save_image(blur, "dog_emboss")

# Gaussian filter
im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-gauss2")

# reconstruct
im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
lfreq = convolve_image(im, f, 1)
hfreq = im - lfreq
reconstruct = lfreq + hfreq
save_image(lfreq, "low-frequency")
save_image(hfreq, "high-frequency")
save_image(reconstruct, "reconstruct")

# ronbledore construct
im = load_image("data/ron.png")
f = make_gaussian_filter(2)
lfreq = convolve_image(im, f, 1)

im1 = load_image("data/dumbledore.png")
f = make_gaussian_filter(4)
lfreq1 = convolve_image(im1, f, 1)

hfreq = im - lfreq
ronbledore = lfreq1 + hfreq
clamp_image(ronbledore)
save_image(ronbledore, "ronbledore")

# SObel filter
im = load_image("data/dog.jpg")
res = sobel_image(im)
mag = res[0]
feature_normalize(mag)
save_image(mag, "magnitude")
feature_normalize(res[1])
save_image(res[1], "theta")

# colorized sobel
im = load_image("data/dog.jpg")
colorized_sobel = colorize_sobel(im)
save_image(colorized_sobel, "colorized-sobel")


