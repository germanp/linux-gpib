/***************************************************************************
                          nec7210/tnt4882_init.c  -  description
                             -------------------
 board specific initialization stuff for National Instruments boards
 using tnt4882 or compatible chips (at-gpib, etc).

    begin                : Dec 2001
    copyright            : (C) 2001, 2002 by Frank Mori Hess
    email                : fmhess@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tnt4882.h"
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/dma.h>
#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/string.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

int ni_isa_attach(gpib_device_t *device);
int ni_pci_attach(gpib_device_t *device);

void ni_isa_detach(gpib_device_t *device);
void ni_pci_detach(gpib_device_t *device);

// wrappers for interface functions
ssize_t tnt4882_read(gpib_device_t *device, uint8_t *buffer, size_t length, int *end)
{
	tnt4882_private_t *priv = device->private_data;
	return nec7210_read(device, &priv->nec7210_priv, buffer, length, end);
}
ssize_t tnt4882_write(gpib_device_t *device, uint8_t *buffer, size_t length, int send_eoi)
{
	tnt4882_private_t *priv = device->private_data;
	return nec7210_write(device, &priv->nec7210_priv, buffer, length, send_eoi);
}
ssize_t tnt4882_command(gpib_device_t *device, uint8_t *buffer, size_t length)
{
	tnt4882_private_t *priv = device->private_data;
	return nec7210_command(device, &priv->nec7210_priv, buffer, length);
}
int tnt4882_take_control(gpib_device_t *device, int synchronous)
{
	tnt4882_private_t *priv = device->private_data;
	return nec7210_take_control(device, &priv->nec7210_priv, synchronous);
}
int tnt4882_go_to_standby(gpib_device_t *device)
{
	tnt4882_private_t *priv = device->private_data;
	return nec7210_go_to_standby(device, &priv->nec7210_priv);
}
void tnt4882_interface_clear(gpib_device_t *device, int assert)
{
	tnt4882_private_t *priv = device->private_data;
	nec7210_interface_clear(device, &priv->nec7210_priv, assert);
}
void tnt4882_remote_enable(gpib_device_t *device, int enable)
{
	tnt4882_private_t *priv = device->private_data;
	nec7210_remote_enable(device, &priv->nec7210_priv, enable);
}
void tnt4882_enable_eos(gpib_device_t *device, uint8_t eos_byte, int compare_8_bits)
{
	tnt4882_private_t *priv = device->private_data;
	nec7210_enable_eos(device, &priv->nec7210_priv, eos_byte, compare_8_bits);
}
void tnt4882_disable_eos(gpib_device_t *device)
{
	tnt4882_private_t *priv = device->private_data;
	nec7210_disable_eos(device, &priv->nec7210_priv);
}
unsigned int tnt4882_update_status(gpib_device_t *device)
{
	tnt4882_private_t *priv = device->private_data;
	return nec7210_update_status(device, &priv->nec7210_priv);
}
void tnt4882_primary_address(gpib_device_t *device, unsigned int address)
{
	tnt4882_private_t *priv = device->private_data;
	nec7210_primary_address(device, &priv->nec7210_priv, address);
}
void tnt4882_secondary_address(gpib_device_t *device, unsigned int address, int enable)
{
	tnt4882_private_t *priv = device->private_data;
	nec7210_secondary_address(device, &priv->nec7210_priv, address, enable);
}
int tnt4882_parallel_poll(gpib_device_t *device, uint8_t *result)
{
	tnt4882_private_t *priv = device->private_data;
	return nec7210_parallel_poll(device, &priv->nec7210_priv, result);
}
int tnt4882_serial_poll_response(gpib_device_t *device, uint8_t status)
{
	tnt4882_private_t *priv = device->private_data;
	return nec7210_serial_poll_response(device, &priv->nec7210_priv, status);
}

gpib_interface_t ni_pci_interface =
{
	name: "ni_pci",
	attach: ni_pci_attach,
	detach: ni_pci_detach,
	read: tnt4882_read,
	write: tnt4882_write,
	command: tnt4882_command,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	line_status: NULL,	//XXX
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
};

gpib_interface_t ni_isa_interface =
{
	name: "atgpib",
	attach: ni_isa_attach,
	detach: ni_isa_detach,
	read: tnt4882_read,
	write: tnt4882_write,
	command: tnt4882_command,
	take_control: tnt4882_take_control,
	go_to_standby: tnt4882_go_to_standby,
	interface_clear: tnt4882_interface_clear,
	remote_enable: tnt4882_remote_enable,
	enable_eos: tnt4882_enable_eos,
	disable_eos: tnt4882_disable_eos,
	parallel_poll: tnt4882_parallel_poll,
	line_status: NULL,	//XXX
	update_status: tnt4882_update_status,
	primary_address: tnt4882_primary_address,
	secondary_address: tnt4882_secondary_address,
	serial_poll_response: tnt4882_serial_poll_response,
};

int tnt4882_allocate_private(gpib_device_t *device)
{
	device->private_data = kmalloc(sizeof(tnt4882_private_t), GFP_KERNEL);
	if(device->private_data == NULL)
		return -1;
	memset(device->private_data, 0, sizeof(tnt4882_private_t));
	return 0;
}

void tnt4882_free_private(gpib_device_t *device)
{
	if(device->private_data)
	{
		kfree(device->private_data);
		device->private_data = NULL;
	}
}

int ni_pci_attach(gpib_device_t *device)
{
	tnt4882_private_t *tnt_priv;
	nec7210_private_t *nec_priv;
	int isr_flags = 0;

	device->status = 0;

	if(tnt4882_allocate_private(device))
		return -ENOMEM;
	tnt_priv = device->private_data;
	nec_priv = &tnt_priv->nec7210_priv;
	nec_priv->read_byte = nec7210_iomem_read_byte;
	nec_priv->write_byte = nec7210_iomem_write_byte;
	nec_priv->offset = atgpib_reg_offset;

	if(mite_devices == NULL)
	{
		printk("no National Instruments PCI boards found\n");
		return -1;
	}

	for(tnt_priv->mite = mite_devices; tnt_priv->mite; tnt_priv->mite = tnt_priv->mite->next)
	{
		if(mite_device_id(tnt_priv->mite) == PCI_DEVICE_ID_NI_GPIB) break;
	}
	if(tnt_priv->mite == NULL)
	{
		printk("no NI PCI-GPIB boards found\n");
		return -1;
	}

	if(mite_setup(tnt_priv->mite) < 0)
	{
		printk("error setting up mite");
		return -1;
	}

	nec_priv->iobase = mite_iobase(tnt_priv->mite);

	// get irq
	if(request_irq(mite_irq(tnt_priv->mite), tnt4882_interrupt, isr_flags, "ni-pci-gpib", device))
	{
		printk("gpib: can't request IRQ %d\n", device->ibirq);
		return -1;
	}
	tnt_priv->irq = mite_irq(tnt_priv->mite);

	/* NAT 4882 reset */
	udelay(1);
	writeb(SFTRST, nec_priv->iobase + CMDR);	/* Turbo488 software reset */
	udelay(1);
	writeb(SETSC, nec_priv->iobase + CMDR);	

	// turn off one-chip mode
	writeb(NODMA, nec_priv->iobase + HSSEL);

	// make sure we are in 7210 mode
	writeb(AUX_7210, nec_priv->iobase + AUXCR);
	udelay(1);
	// registers might be swapped, so write it to the swapped address too
	writeb(AUX_7210, nec_priv->iobase +  SWAPPED_AUXCR);
	udelay(1);

	nec7210_board_reset(nec_priv);

	// turn off one chip mode and dma
	writeb(0x10, nec_priv->iobase + 0xd);	

	// enable passing of nec7210 interrupts
	writeb(0x2, nec_priv->iobase + IMR3);

	// enable interrupt
	writeb(0x1, nec_priv->iobase + INTRT);

	// enable nec7210 interrupts
	nec_priv->imr1_bits = HR_ERRIE | HR_DECIE | HR_ENDIE |
		HR_DETIE | HR_APTIE | HR_CPTIE | HR_DOIE | HR_DIIE;
	nec_priv->imr2_bits = IMR2_ENABLE_INTR_MASK;
	write_byte(nec_priv, nec_priv->imr1_bits, IMR1);
	write_byte(nec_priv, nec_priv->imr2_bits, IMR2);

	write_byte(nec_priv, AUX_PON, AUXMR);

	return 0;
}

void ni_pci_detach(gpib_device_t *device)
{
	tnt4882_private_t *tnt_priv = device->private_data;
	nec7210_private_t *nec_priv;

	if(tnt_priv)
	{
		nec_priv = &tnt_priv->nec7210_priv;
		if(tnt_priv->irq)
		{
			free_irq(tnt_priv->irq, device);
		}
		if(nec_priv->iobase)
		{
			nec7210_board_reset(nec_priv);
		}
		if(tnt_priv->mite)
			mite_unsetup(tnt_priv->mite);
	}
	tnt4882_free_private(device);
}

int ni_isa_attach(gpib_device_t *device)
{
	tnt4882_private_t *tnt_priv;
	nec7210_private_t *nec_priv;
	int isr_flags = 0;

	device->status = 0;

	if(tnt4882_allocate_private(device))
		return -ENOMEM;
	tnt_priv = device->private_data;
	nec_priv = &tnt_priv->nec7210_priv;
	nec_priv->read_byte = nec7210_ioport_read_byte;
	nec_priv->write_byte = nec7210_ioport_write_byte;
	nec_priv->offset = atgpib_reg_offset;

	// allocate ioports
	if(check_region(device->ibbase, atgpib_iosize) < 0)
	{
		printk("gpib: ioports are already in use");
		return -1;
	}
	request_region(device->ibbase, atgpib_iosize, "atgpib");
	nec_priv->iobase = device->ibbase;

	// get irq
	if(request_irq(device->ibirq, tnt4882_interrupt, isr_flags, "atgpib", device))
	{
		printk("gpib: can't request IRQ %d\n", device->ibirq);
		return -1;
	}
	tnt_priv->irq = device->ibirq;

	/* NAT 4882 reset */
	udelay(1);
	outb(SFTRST, nec_priv->iobase + CMDR);	/* Turbo488 software reset */
	udelay(1);
	writeb(SETSC, nec_priv->iobase + CMDR);	

	// turn off one-chip mode
	writeb(NODMA, nec_priv->iobase + HSSEL);

	// make sure we are in 7210 mode
	writeb(AUX_7210, nec_priv->iobase + AUXCR);
	udelay(1);
	// registers might be swapped, so write it to the swapped address too
	writeb(AUX_7210, nec_priv->iobase +  SWAPPED_AUXCR);
	udelay(1);

	nec7210_board_reset(nec_priv);

	// enable passing of nec7210 interrupts
	outb(0x2, nec_priv->iobase + IMR3);

	// enable interrupt
	outb(0x1, nec_priv->iobase + INTRT);

	// enable nec7210 interrupts
	nec_priv->imr1_bits = HR_ERRIE | HR_DECIE | HR_ENDIE |
		HR_DETIE | HR_APTIE | HR_CPTIE | HR_DOIE | HR_DIIE;
	nec_priv->imr2_bits = IMR2_ENABLE_INTR_MASK;
	write_byte(nec_priv, nec_priv->imr1_bits, IMR1);
	write_byte(nec_priv, nec_priv->imr2_bits, IMR2);

	write_byte(nec_priv, AUX_PON, AUXMR);

	return 0;
}

void ni_isa_detach(gpib_device_t *device)
{
	tnt4882_private_t *tnt_priv = device->private_data;
	nec7210_private_t *nec_priv;

	if(tnt_priv)
	{
		nec_priv = &tnt_priv->nec7210_priv;
		if(tnt_priv->irq)
		{
			free_irq(tnt_priv->irq, device);
		}
		if(nec_priv->iobase)
		{
			nec7210_board_reset(nec_priv);
			release_region(nec_priv->iobase, atgpib_iosize);
		}
	}
	tnt4882_free_private(device);
}

int init_module(void)
{
	EXPORT_NO_SYMBOLS;

	INIT_LIST_HEAD(&ni_isa_interface.list);
	INIT_LIST_HEAD(&ni_pci_interface.list);

	gpib_register_driver(&ni_isa_interface);
	gpib_register_driver(&ni_pci_interface);

	mite_init();
	mite_list_devices();

	return 0;
}

void cleanup_module(void)
{
	gpib_unregister_driver(&ni_isa_interface);
	gpib_unregister_driver(&ni_pci_interface);

	mite_cleanup();
}









