import { Response } from 'express';
import { Controller, Get, Param, Res } from '@nestjs/common';
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
  getHostActivityStatus(@Param() { id }) {
    const findActiveHost = this.hostsService.activeHosts.find(
      (activeHost) => activeHost.number === +id,
    );
    if (findActiveHost) {
      return 1;
    } else {
      return 0;
    }
  }
}
