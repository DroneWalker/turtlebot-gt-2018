# from __future__ import print_function, division
import torch
import torch.nn.functional as F
from torch.autograd import Variable
from torch.utils.data import Dataset, DataLoader
from torchvision import transforms, utils
from torch.utils.data.sampler import SubsetRandomSampler
import torch.optim as optim
from torch.autograd import Variable

from torchvision import datasets, models, transforms
import matplotlib
import numpy as np
matplotlib.use('TkAgg')

import os
import time
from src.convnet_c4f4 import ConvNet_C4F4
# from src.convnet_c4f4 import ConvNet_C4F4_v2
# from src.simplecnn import SimpleCNN

def main():

    cwd = os.getcwd()

    model = ConvNet_C4F4()
    # model = ConvNet_C4F4_v2()
    # model.load_state_dict(torch.load('Learning_models/CNN4layers_fc4layers_raw_epoch30_new_dataset.pt', map_location='cpu'))

    size_test_batch = 10

    transformations = transforms.Compose([
        transforms.Resize((32, 32)),
        # transforms.CenterCrop(32),
        transforms.ToTensor(),
        transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])])

    image_folder = cwd + '/cropped_test_data'
    training_set = datasets.ImageFolder(root=image_folder, transform=transformations)
    # testset = torchvision.datasets.ImageFolder(image_folder)

    # Training
    n_training_samples = 20000
    train_sampler = SubsetRandomSampler(np.arange(n_training_samples, dtype=np.int64))
    training_loader = torch.utils.data.DataLoader(training_set, batch_size=32, shuffle=False, num_workers=1)

    val_loader = torch.utils.data.DataLoader(training_set,batch_size=10, shuffle=False, num_workers=1)

    # Validation
    n_val_samples = 5000
    val_sampler = SubsetRandomSampler(np.arange(n_val_samples, dtype=np.int64))

    # Test
    n_test_samples = 5000
    test_sampler = SubsetRandomSampler(np.arange(n_test_samples, dtype=np.int64))

    # # DataLoader takes in a dataset and a sampler for loading (num_workers deals with system level memory)
    # def get_train_loader(batch_size, train_data, train_sampler):
    #     train_loader = torch.utils.data.DataLoader(train_data, batch_size=batch_size,
    #                                            sampler=train_sampler, num_workers=2)
    #     return(train_loader)


    # # Test and validation loaders have constant batch sizes, so we can define them directly
    # test_loader = torch.utils.data.DataLoader(training_set, batch_size=4, sampler=test_sampler, num_workers=2)
    # # val_loader = torch.utils.data.DataLoader(training_dataset, batch_size=128, sampler=val_sampler, num_workers=2)


    def createLossAndOptimizer(net, learning_rate=0.001):

        # Loss Function
        loss = torch.nn.CrossEntropyLoss()

        # Optimizer
        optimizer = optim.Adam(net.parameters(), lr = learning_rate)

        return(loss, optimizer)

    def TrainNet(net, batch_size, n_epochs, learning_rate):

        # Print all of the hyperparameters of the training iteration:
        print("===== HYPERPARAMETERS =====")
        print("batch_size=", batch_size)
        print("epochs=", n_epochs)
        print("learning_rate=", learning_rate)
        print("=" * 30)

        # Get training data
        train_loader = training_loader
        n_batches = len(train_loader)

        # Create our loss and optimizer functions
        loss, optimizer = createLossAndOptimizer(net, learning_rate)

        # Time for printing
        training_start_time = time.time()

        # Loop for n_epochs
        for epoch in range(n_epochs):

            running_loss = 0.0
            print_every = n_batches // 10
            start_time = time.time()
            total_train_loss = 0

            for i, data in enumerate(train_loader, 0):

                # Get inputs
                inputs, labels = data

                # Wrap them in a Variable object
                inputs, labels = Variable(inputs), Variable(labels)

                # Set the parameter gradients to zero
                optimizer.zero_grad()

                # Forward pass, backward pass, optimize
                outputs = net(inputs)
                loss_size = loss(outputs, labels)
                loss_size.backward()
                optimizer.step()

                # Print statistics
                running_loss += loss_size.data[0]
                total_train_loss += loss_size.data[0]

                # Print every 10th batch of an epoch
                if (i + 1) % (print_every + 1) == 0:
                    print("Epoch {}, {:d}% \t train_loss: {:.2f} took: {:.2f}s".format(
                        epoch + 1, int(100 * (i + 1) / n_batches), running_loss / print_every,
                        time.time() - start_time))
                    # Reset running loss and time
                    running_loss = 0.0
                    start_time = time.time()

            # At the end of the epoch, do a pass on the validation set
            total_val_loss = 0
            for inputs, labels in val_loader:
                # Wrap tensors in Variables
                inputs, labels = Variable(inputs), Variable(labels)

                # Forward pass
                val_outputs = net(inputs)
                val_loss_size = loss(val_outputs, labels)
                total_val_loss += val_loss_size.data[0]

            print("Validation loss = {:.2f}".format(total_val_loss / len(val_loader)))

        print("Training finished, took {:.2f}s".format(time.time() - training_start_time))

    # CNN = convnet_c4f4.ConvNet_C4F4()
    TrainNet(model, batch_size = 32, n_epochs=100, learning_rate = 0.0001)
    torch.save(model.state_dict(), 'Learning_models/cnn_trained_best_94.pt')

if __name__ == "__main__":
    main()



