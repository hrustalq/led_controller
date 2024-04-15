import { Response } from 'express';
import { Controller, Get, NotFoundException, Param, Res } from '@nestjs/common';
import { HostsService } from './hosts.service';

@Controller('hosts')
export class HostsController {
  constructor(private hostsService: HostsService) {}

  @Get('all')
  getAllHosts(@Res() res: Response) {
    res.header('Access-Control-Allow-Origin', '*');
    res.status(200).json(this.hostsService.hosts);
  }
  @Get(':id/status')
  getHostActivityStatus(@Param() { id }, @Res() res: Response) {
    const active = this.hostsService.findActive(+id);
    if (!isNaN(active)) {
      res.status(200).send(`${active}`);
    } else  throw new NotFoundException();
  }
}
