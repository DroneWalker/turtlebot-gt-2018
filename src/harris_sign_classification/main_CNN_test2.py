# from __future__ import print_function, division
import torch
import torch.nn.functional as F
from torch.autograd import Variable

from torchvision import datasets, models, transforms
import matplotlib
matplotlib.use('TkAgg')

import os
from src.convnet_c4f4 import ConvNet_C4F4

def main():

    cwd = os.getcwd()


    model = ConvNet_C4F4()
    # model.load_state_dict(torch.load('Learning_models/CNN4layers_fc4layers_raw_epoch30_new_dataset.pt',map_location='cpu'))
    model.load_state_dict(torch.load('Learning_models/cnn_trained_cropped_1.pt',map_location='cpu'))
    SunTag_label = []

    size_test_batch = 10

    transformations = transforms.Compose([
        transforms.Resize((32, 32)),
        # transforms.CenterCrop(32),
        transforms.ToTensor(),
        transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])])

    image_folder = cwd+'/cropped_test_data'
    testset = datasets.ImageFolder(root=image_folder,transform=transformations)
    # testset = torchvision.datasets.ImageFolder(image_folder)
    test_loader = torch.utils.data.DataLoader(testset, batch_size=6,shuffle=False,num_workers=1)

    model.eval()
    test_loss = 0
    correct = 0

    for batch_idx, (data, target) in enumerate(test_loader):
        data, target = Variable(data), Variable(target)
        output = model(data)
        test_loss += F.nll_loss(output, target, size_average=False).data[0]
        # get the index of the max log-probability
        pred = output.data.max(1, keepdim=True)[1]
        correct += pred.eq(target.data.view_as(pred)).cpu().sum()

        test_loss /= len(test_loader.dataset)
    print('Test set: Average loss: {:.4f}, Accuracy: {}/{} ({:.0f}%)'.format(
        test_loss, correct, len(test_loader.dataset),
        100. * correct / len(test_loader.dataset)))


if __name__ == "__main__":
    main()